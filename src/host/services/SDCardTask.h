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
#include "os/Task.h"
#include "signalk/KMessage.h"

class Loggable {
  public:
    Loggable(String s, String m) : _source(s), _message(m), timestamp(millis()) {};
    String _source;
    String _message;
    uint32_t timestamp;
};

class SDCardTask : public Task, public KReceiver {
  private:
    #if defined(__MK66FX1M0__) // Teensy 3.6
      // For Builtin SD-Card in Teensy 3.6
      SdFatSdio *sd = 0;  // Teensy 3.5/3.6 SDIO support lt. README.md SDFat Library
    #else
      // KBox, Teensy 3.2
      SdFat *sd = 0;
    #endif

    SdFile *logFile = 0;
    bool cardReady = false;

    bool cardInit();
    String generateNewFileName(const String& baseName);
    SdFile* createLogFile(const String& baseName);

    LinkedList<Loggable> receivedMessages;

  public:
    SDCardTask();
    virtual ~SDCardTask();

    void setup();
    void loop();
    void processMessage(const KMessage&);

    // Those two functions returns value in bytes.
    uint64_t getFreeSpace() const;
    uint32_t getLogSize() const;

    bool isLogging() const;
    String getLogFileName() const;
};
