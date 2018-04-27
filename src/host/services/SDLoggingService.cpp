/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "SDLoggingService.h"
#include "../config/KBoxConfig.h"

#include <KBoxLogging.h>
#include <KBoxHardware.h>
#include <Seasmart.h>
#include <common/time/WallClock.h>

SDLoggingService::SDLoggingService(const SDLoggingConfig &config) : Task("SDCard"), _config(config) {
}

void SDLoggingService::setup() {
  if (KBox.isSdCardUsable()) {
    cardReady = true;

    // Takes a long time to count free clusters so only do it once at startup.
    _freeSpaceAtBoot = KBox.getSdFat().vol()->freeClusterCount();
    _freeSpaceAtBoot *= logFile->volume()->blocksPerCluster();
    _freeSpaceAtBoot *= 512;
  }
}

void SDLoggingService::startLogging() {
  if (isLogging() || !_config.enabled || !KBox.isSdCardUsable()) {
    return;
  }

  String fileName;
  if (_config.logWithoutTime) {
    // start a new logfile with a number
    fileName = generateNewFileName("kbox-");
  }
  else if (wallClock.isTimeSet()) {
    // start a new logfile with date and time
    fileName = "kbox-";
    fileName += wallClock.now().iso8601date();
    fileName += "-";
    fileName += wallClock.now().iso8601basicTime();
    fileName += "Z.log";
  }

  if (fileName.length() > 0) {
    logFile = createLogFile(fileName);

    if (logFile) {
      DEBUG("Starting new logfile: %s", fileName.c_str());
    }
    else if (fileName) {
      DEBUG("Unable to create logfile %s", fileName.c_str());
    }
  }
}

void SDLoggingService::loop() {
  // Try to start logging if we are not already.
  // If it fails, clear messages and bails.
  if (!isLogging()) {
    startLogging();
    if (!isLogging()) {
      receivedMessages.clear();
      return;
    }
  }
  for (LinkedList<Loggable>::iterator it = receivedMessages.begin(); it != receivedMessages.end(); it++) {
    logFile->print(it->timestamp);
    logFile->print(",");
    logFile->print(it->_message);
    logFile->println();
  }
  // Force data to SD and update the directory entry to avoid data loss.
  if (!logFile->sync() || logFile->getWriteError()) {
    DEBUG("Logfile write error: %i", logFile->Print::getWriteError());
  }
  // We always clear the list anyway.
  receivedMessages.clear();
}


String SDLoggingService::generateNewFileName(const String& baseName) {
  char fileName[20];
  for (int i = 0; i < 99999; i++) {
    snprintf(fileName, sizeof(fileName), "%s%i.log", baseName.c_str(), i);
    if (!KBox.getSdFat().exists(fileName)) {
      return String(fileName);
    }
  }
  DEBUG("Unable to create new file. Delete some older files!");
  return "";
}

SdFile* SDLoggingService::createLogFile(const String& fileName) {
  if (!cardReady) {
    return nullptr;
  }

  SdFile *file = new SdFile();
  if (fileName == "" || !file->open(fileName.c_str(), O_CREAT | O_WRITE | O_EXCL)) {
    DEBUG("Error while opening file '%s'", fileName.c_str());
    return nullptr;
  }
  return file;
}

uint64_t SDLoggingService::getFreeSpace() const {
  return _freeSpaceAtBoot - getLogSize();
}

bool SDLoggingService::isLogging() const {
  return cardReady && logFile;
}

uint32_t SDLoggingService::getLogSize() const {
  if (!isLogging()) {
    return 0;
  }
  return logFile->fileSize();
}

String SDLoggingService::getLogFileName() const {
  if (!isLogging()) {
    return String();
  }
  char name[50];
  logFile->getName(name, sizeof(name));
  return String(name);
}

bool SDLoggingService::write(const SKNMEASentence &nmeaSentence) {
  if (!isLogging()) {
    return true;
  }

  receivedMessages.add(Loggable("", nmeaSentence));
  return true;
}

bool SDLoggingService::write(const tN2kMsg &msg) {
  if (!isLogging()) {
    return true;
  }

  if (msg.DataLen > 500) {
    return false;
  }

  char pcdin[30 + msg.DataLen * 2];
  if (N2kToSeasmart(msg, millis(), pcdin, sizeof(pcdin)) < sizeof(pcdin)) {
    receivedMessages.add(Loggable("", pcdin));
    return true;
  } else {
    return false;
  }
}
