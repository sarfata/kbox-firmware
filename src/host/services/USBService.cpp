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

#include <Arduino.h>
#include <KBoxLogging.h>
#include <KBoxHardware.h>
#include <comms/Kommand.h>
#include "../esp-programmer/ESPProgrammer.h"
#include "../drivers/ILI9341GC.h"

#include "USBService.h"

USBService::USBService(GC &gc) : Task("USBService"), _slip(Serial, 2048),
  _streamLogger(KBoxLoggerStream(Serial)),
  _kommandContext(_slip, gc), _state(ConnectedDebug) {
}

void USBService::setup() {
}

void USBService::log(enum KBoxLoggingLevel level, const char *fname, int lineno, const char *fmt, va_list fmtargs) {
  switch (_state) {
    case ConnectedDebug:
      _streamLogger.log(level, fname, lineno, fmt, fmtargs);
      break;
    case ConnectedFrame:
      sendLogFrame(level, fname, lineno, fmt, fmtargs);
      break;
    case ConnectedESPProgramming:
      /* Discard all logs when in ESP programming mode. */
      break;
  }
}

void USBService::loop() {
  /* Switching serial port to 230400 on computer signals that the host
   * wants to go into default debugging mode.
   */
  if (Serial.baud() == 115200) {
    _state = ConnectedDebug;
  }

  /* Switching serial port to 230400 on computer signals that the host
   * wants to go into ESP Programming mode.
   */
  if (Serial.baud() == 230400) {
    _state = ConnectedESPProgramming;
  }

  /* Switching serial port to 1mbits on computer signals that the host
   * wants to go into framed mode.
   */
  if (Serial.baud() == 1000000) {
    _state = ConnectedFrame;
  }

  switch (_state) {
    case ConnectedDebug:
      /* nop */
      break;
    case ConnectedFrame:
      loopConnectedFrame();
      break;
    case ConnectedESPProgramming:
      loopConnectedESPProgramming();
      break;
  };
}

/**
 * In frame mode we look at incoming frames to receive commands and
 * execute them. The commands that can be receied over USB are very
 * similar to commands that can be received via WiFi module so the same
 * @class KommandContext is used to receive and process them.
 */
void USBService::loopConnectedFrame() {
  if (_slip.available()) {
    uint8_t *frame;
    size_t len = _slip.peekFrame(&frame);

    _kommandContext.process(frame, len);

    // Discard the frame.
    _slip.readFrame(0, 0);
  }
}

class ESPProgrammerDelegateImpl : public ESPProgrammerDelegate {
  void rebootInFlasher() {
    KBox.espRebootInFlasher();
  };
};

/**
 * Forwards all messages between host computer and ESP8266 module so that a
 * standard ESP8266 program can be used.
 *
 * This is a bit more complicated that it sounds because the programmer will
 * switch speed half-way through so we need to read the first packets to follow
 * that (see @class ESPProgrammer).
 */
void USBService::loopConnectedESPProgramming() {
  ESPProgrammerDelegateImpl delegate;
  ESPProgrammer programmer(KBox.getNeopixels(), Serial, Serial1, delegate);

  while (programmer.getState() != ESPProgrammer::ProgrammerState::Done) {
    programmer.loop();
  }
  DEBUG("Exiting programmer mode");
  CPU_RESTART;
}

/**
 * Sends a frame with a logging message.
 */
void USBService::sendLogFrame(KBoxLoggingLevel level, const char *fname, int lineno, const char *fmt, va_list fmtargs) {
  Kommand<MaxLogFrameSize> logKmd(KommandLog);
  logKmd.append16(level);
  logKmd.append16(lineno);
  logKmd.append16(strlen(fname));
  logKmd.appendNullTerminatedString(fname);

  uint8_t *bytes;
  size_t *index;

  logKmd.captureBuffer(&bytes, &index);

  *index += vsnprintf((char*)bytes + *index, MaxLogFrameSize - *index, fmt, fmtargs);

  _slip.writeFrame(bytes, *index);
}
