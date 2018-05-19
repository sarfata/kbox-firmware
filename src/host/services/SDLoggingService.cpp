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
#include <ArduinoJson.h>
#include "common/time/WallClock.h"
#include "common/time/WallClock.h"
#include "common/signalk/SKJSONVisitor.h"

SDLoggingService::SDLoggingService(const SDLoggingConfig &config, SKHub &hub) :
  Task("SDCard"), _config(config), _hub(hub) {
}

static void dateTime(uint16_t* date, uint16_t* time) {
  SKTime t = wallClock.now();

  *date = t.getFatDate();
  *time = t.getFatTime();
}

void SDLoggingService::setup() {
  if (KBox.isSdCardUsable()) {
    cardReady = true;

    // Takes a long time to count free clusters so only do it once at startup.
    _freeSpaceAtBoot = KBox.getSdFat().vol()->freeClusterCount();
    _freeSpaceAtBoot *= KBox.getSdFat().vol()->blocksPerCluster();
    _freeSpaceAtBoot *= 512;
  }

  _hub.subscribe(this);

  // Tell SDFat how to get the current time
  SdFile::dateTimeCallback(dateTime);
}

void SDLoggingService::startLogging() {
  if (isLogging() || !_config.enabled || !KBox.isSdCardUsable() || getFreeSpace() < MinimumFreeSpace) {
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
    createLogFile(fileName);

    if (logFile) {
      INFO("New logfile by KBox %s - Reboot reason %s", KBOX_VERSION, KBox.rebootReason().c_str());
      DEBUG("Starting new logfile: %s", fileName.c_str());
    }
    else {
      DEBUG("Unable to create logfile %s", fileName.c_str());
    }
  }
}

void SDLoggingService::loop() {
  // Make sure file is not getting out of hand.
  if (getLogSize() > SDLoggingService::MaximumLogSize || getFreeSpace() < MinimumFreeSpace) {
    rotateLogfile();
  }

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
    logFile.print(it->_timestamp.getTime());
    if (it->_timestamp.hasMilliseconds() && it->_timestamp.getMilliseconds() != 0) {
      if (it->_timestamp.getMilliseconds() < 100) {
        logFile.print("0");
      }
      if (it->_timestamp.getMilliseconds() < 10) {
        logFile.print("0");
      }
      logFile.print(it->_timestamp.getMilliseconds());
    }
    else {
      logFile.print("000");
    }
    logFile.print(";");
    logFile.print(it->_source);
    logFile.print(";");
    logFile.print(it->_message);
    logFile.println();
  }
  // Force data to SD and update the directory entry to avoid data loss.
  if (!logFile.sync() || logFile.getWriteError()) {
    DEBUG("Logfile write error");
    rotateLogfile();
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

void SDLoggingService::createLogFile(const String& fileName) {
  if (!cardReady) {
    return;
  }

  logFile = KBox.getSdFat().open(fileName, O_CREAT | O_WRITE | O_EXCL);
  if (!logFile) {
    DEBUG("Error while opening file '%s'", fileName.c_str());
  }
}

uint64_t SDLoggingService::getFreeSpace() {
  return _freeSpaceAtBoot - getLogSize();
}

bool SDLoggingService::isLogging() {
  return cardReady && logFile && logFile.isOpen();
}

uint32_t SDLoggingService::getLogSize() {
  if (!isLogging()) {
    return 0;
  }
  return logFile.fileSize();
}

String SDLoggingService::getLogFileName() {
  if (!isLogging()) {
    return String();
  }
  char name[50];
  logFile.getName(name, sizeof(name));
  return String(name);
}

bool SDLoggingService::write(const SKNMEASentence &nmeaSentence) {
  if (!isLogging() || !_config.logNMEA) {
    return true;
  }

  receivedMessages.add(Loggable("N", nmeaSentence, wallClock.now()));
  return true;
}

bool SDLoggingService::write(const tN2kMsg &msg) {
  if (!isLogging() || !_config.logNMEA2000) {
    return true;
  }

  if (msg.DataLen > 500) {
    return false;
  }

  char pcdin[30 + msg.DataLen * 2];
  if (N2kToSeasmart(msg, wallClock.now().getTime(), pcdin, sizeof(pcdin)) < sizeof(pcdin)) {
    receivedMessages.add(Loggable("P", pcdin, wallClock.now()));
    return true;
  } else {
    return false;
  }
}

void SDLoggingService::updateReceived(const SKUpdate &update) {
  if (!isLogging() || !_config.logSignalK) {
    return;
  }

  // Do not log NMEA messages that have been converted to SignalK unless requested by user.
  if (!_config.logSignalKGeneratedFromNMEA &&
      (update.getSource().getInput() == SKSourceInputNMEA0183_1
       || update.getSource().getInput() == SKSourceInputNMEA0183_2)) {
    return;
  }
  // Same for NMEA2000
  if (!_config.logSignalKGeneratedFromNMEA2000 && update.getSource().getInput() == SKSourceInputNMEA2000) {
    return;
  }
  // And for KBox Sensors
  if (!_config.logSignalKGeneratedByKBoxSensors &&
      (update.getSource().getInput() == SKSourceInputKBoxADC
       || update.getSource().getInput() == SKSourceInputKBoxBarometer
       || update.getSource().getInput() == SKSourceInputKBoxIMU)) {
    return;
  }

  StaticJsonBuffer<1024> jsonBuffer;
  SKJSONVisitor jsonVisitor("self", jsonBuffer);
  JsonObject &jsonData = jsonVisitor.processUpdate(update);

  String jsonString;
  jsonData.printTo(jsonString);

  receivedMessages.add(Loggable("I", jsonString, wallClock.now()));
}

void SDLoggingService::rotateLogfile() {
  if (!isLogging()) {
    return;
  }

  _freeSpaceAtBoot = _freeSpaceAtBoot - logFile.fileSize();
  logFile.close();
}

void SDLoggingService::log(enum KBoxLoggingLevel level, const char *filename, int lineNumber, const char *fmt,
                           va_list fmtargs) {
  if (!isLogging() || !_config.logSystemMessages) {
    return;
  }

  if (level == KBoxLoggingLevelDebug || level == KBoxLoggingLevelESPDebug) {
    // do not save debug messages
    return;
  }

  static const char *logLevelPrefixes[] = { "LHD", "LHI", "LHE", "LWD", "LWI", "LWE" };

  String message = filename;
  message += ":";
  message += lineNumber;

  char tmp[128];
  vsnprintf(tmp, sizeof(tmp), fmt, fmtargs);

  message += "|";
  message += tmp;

  receivedMessages.add(Loggable(logLevelPrefixes[level], message, wallClock.now()));
}
