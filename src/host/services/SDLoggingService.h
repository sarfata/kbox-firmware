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

#pragma once

#include <SdFat.h>
#include <KBoxLogging.h>
#include "common/signalk/SKNMEAOutput.h"
#include "common/signalk/SKNMEA2000Output.h"
#include "common/signalk/SKSubscriber.h"
#include "common/signalk/SKHub.h"
#include "common/signalk/SKTime.h"
#include "common/algo/List.h"
#include "common/os/Task.h"
#include "host/config/SDLoggingConfig.h"

class Loggable {
  public:
    Loggable(String s, String m, SKTime timestamp) : _source(s), _message(m), _timestamp(timestamp) {};
    String _source;
    String _message;
    SKTime _timestamp;
};

class SDLoggingService : public Task, public SKNMEAOutput, public SKNMEA2000Output, public SKSubscriber,
  public KBoxLogger {
  private:
    uint64_t _freeSpaceAtBoot;
    File logFile;
    bool cardReady = false;
    const SDLoggingConfig &_config;
    SKHub &_hub;
    // Limit log size to 1 GB.
    static const uint32_t MaximumLogSize = 1024 * 1024 * 1024 * 1;
    // We will not log if free space is below 100 kB
    static const uint32_t MinimumFreeSpace = 1024 * 100;

    String generateNewFileName(const String& baseName);
    void createLogFile(const String& baseName);
    void rotateLogfile();

    LinkedList<Loggable> receivedMessages;

  public:
    SDLoggingService(const SDLoggingConfig &config, SKHub &hub);
    virtual ~SDLoggingService() = default;

    void setup() override;
    void loop() override;

    // Those two functions returns value in bytes.
    uint64_t getFreeSpace();
    uint32_t getLogSize();

    bool isLogging();
    String getLogFileName();

    bool write(const SKNMEASentence &nmeaSentence) override;
    bool write(const tN2kMsg &m) override;
    void updateReceived(const SKUpdate &update) override;

    void startLogging();

    void
    log(enum KBoxLoggingLevel level, const char *filename, int lineNumber, const char *fmt, va_list fmtargs) override;
};
