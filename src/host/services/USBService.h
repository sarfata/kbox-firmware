/*
  The MIT License

  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net

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

#include <KBoxLogging.h>
#include <KBoxLoggerStream.h>
#include <common/signalk/SKNMEA2000Output.h>
#include "common/comms/SlipStream.h"
#include "common/os/Task.h"
#include "common/ui/GC.h"
#include "common/comms/SlipStream.h"
#include "common/comms/KommandHandlerPing.h"
#include "common/comms/KommandHandlerScreenshot.h"
#include "common/signalk/SKHub.h"
#include "common/signalk/SKSubscriber.h"
#include "common/signalk/SKNMEAOutput.h"
#include "host/comms/KommandHandlerFileRead.h"
#include "host/comms/KommandHandlerFileWrite.h"
#include "host/comms/KommandHandlerReboot.h"

class USBService : public Task, public KBoxLogger, public SKSubscriber,
                   public SKNMEAOutput, public SKNMEA2000Output {
  private:
    static const size_t MaxLogFrameSize = 256;

    SlipStream _slip;
    KBoxLoggerStream _streamLogger;
    SKHub &_skHub;
    KommandHandlerPing _pingHandler;
    KommandHandlerScreenshot _screenshotHandler;
    KommandHandlerFileRead _fileReadHandler;
    KommandHandlerFileWrite _fileWriteHandler;
    KommandHandlerReboot _rebootHandler;

    enum USBConnectionState{
      ConnectedDebug,
      ConnectedFrame,
      ConnectedESPProgramming,
      ConnectedNMEAInterface
    };
    USBConnectionState _state;

    void loopConnectedFrame();
    void loopConnectedESPProgramming();
    void loopNMEAInterfaceMode();

    void sendLogFrame(KBoxLoggingLevel level, const char *fname, int lineno,
                      const char *fmt, va_list fmtargs);

  public:
    USBService(GC &gc, SKHub &hub);
    ~USBService() {};

    void setup();
    void loop();
    void log(enum KBoxLoggingLevel level, const char *fname, int lineno,
             const char *fmt, va_list args) override;
    void updateReceived(const SKUpdate& u);

    bool write(const SKNMEASentence &nmeaSentence);
    bool write(const tN2kMsg&);
};
