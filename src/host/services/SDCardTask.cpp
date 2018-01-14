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

#include <KBoxLogging.h>
#include <KBoxHardware.h>
#include "signalk/KMessageNMEAVisitor.h"

#include "SDCardTask.h"

SDCardTask::SDCardTask(SDCardConfig &config) : Task("SDCard"),  _config(config) {
}

void SDCardTask::setup() {
  if (KBox.isSdCardUsable()) {
    cardReady = true;
    logFile = createLogFile("kbox-");
    DEBUG("SDCard logging to: %s", getLogFileName().c_str());
  }
}

void SDCardTask::processMessage(const KMessage &m) {
  if (!isLogging()) {
    return;
  }

  // FIXME: Should probably implement a custom visitor for logging.
  KMessageNMEAVisitor v;
  m.accept(v);
  receivedMessages.add(Loggable("", v.getNMEAContent()));
}

void SDCardTask::loop() {
  if (!isLogging()) {
    return;
  }
  for (LinkedList<Loggable>::iterator it = receivedMessages.begin(); it != receivedMessages.end(); it++) {
    if ( _config.writeTimestamp) {
      logFile->print(it->timestamp);
      logFile->print(",");
    }
    logFile->print(it->_message);
  }
  // Force data to SD and update the directory entry to avoid data loss.
  if (!logFile->sync() || logFile->getWriteError()) {
    DEBUG("Logfile write error: %i", logFile->Print::getWriteError());
  }
  // We always clear the list anyway.
  receivedMessages.clear();
}


String SDCardTask::generateNewFileName(const String& baseName) {
  if (baseName.length() > 6) {
    DEBUG("basename too long (%s)", baseName.c_str());
    return "";
  }

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

SdFile* SDCardTask::createLogFile(const String& baseName) {
  if (!cardReady) {
    return nullptr;
  }
  String fileName = generateNewFileName(baseName);

  SdFile *file = new SdFile();
  if (fileName == "" || !file->open(fileName.c_str(), O_CREAT | O_WRITE | O_EXCL)) {
    DEBUG("Error while opening file '%s'", fileName.c_str());
    return nullptr;
  }
  return file;
}

uint64_t SDCardTask::getFreeSpace() const {
  if (!isLogging()) {
    return 0;
  }
  // FIXME: Something weird happens when we call freeClusterCount()
  // the running led stops flashing (although we do still call digitalWrite on it)
  // the serial ports start messing up big time (missing a lot of data)
  // eventually other things crash...
  // Could be a memory problem or something like that. Have not found
  // it yet but the culprit is this line so for now it is disabled.
  uint64_t space = 0; //logFile->volume()->freeClusterCount();
  space *= logFile->volume()->blocksPerCluster();
  space *= 512;
  return space;
}

bool SDCardTask::isLogging() const {
  return cardReady && logFile;
}

uint32_t SDCardTask::getLogSize() const {
  if (!isLogging()) {
    return 0;
  }
  return logFile->fileSize();
}

String SDCardTask::getLogFileName() const {
  if (!isLogging()) {
    return String();
  }
  char name[13];
  logFile->getName(name, sizeof(name));
  return String(name);
}
