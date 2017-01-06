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

#include "USBTask.h"
#include <Arduino.h>
#include <KBoxLogging.h>
#include <KBox.h>
#include "util/ESPProgrammer.h"

USBTask::USBTask() : Task("USBTask"), _slip(Serial, 2048), _streamLogger(KBoxLoggerStream(Serial)), _state(ConnectedDebug) {
}

void USBTask::setup() {

}

void USBTask::log(enum KBoxLoggingLevel level, const char *fname, int lineno, const char *fmt, va_list fmtargs) {
  static const char logLevelPrefixes[3] = { 'D', 'I', 'E' };

  if (_state == ConnectedDebug) {
    _streamLogger.log(level, fname, lineno, fmt, fmtargs);
  }
}

void USBTask::loop() {
  switch (_state) {
    case ConnectedDebug:
      loopConnectedDebug();
      break;
    case ConnectedFrame:
      loopConnectedFrame();
      break;
    case ConnectedESPProgramming:
      loopConnectedESPProgramming();
      break;
  };
}

void USBTask::loopConnectedDebug() {
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
}

/**
 * In frame mode we look at incoming frames to receive commands and
 * execute them. The commands that can be receied over USB are very
 * similar to commands that can be received via WiFi module so the same
 * @class KommandContext is used to receive and process them.
 */
void USBTask::loopConnectedFrame() {
  if (_slip.available()) {
    uint8_t *frame;
    size_t len = _slip.peekFrame(&frame);

    //kommandContext.process(frame, len);

    // Discard the frame.
    _slip.readFrame(0, 0);
  }
}

/**
 * Forwards all messages between host computer and ESP8266 module so that a
 * standard ESP8266 program can be used.
 *
 * This is a bit more complicated that it sounds because the programmer will
 * switch speed half-way through so we need to read the first packets to follow
 * that (see @class ESPProgrammer).
 */
void USBTask::loopConnectedESPProgramming() {
  ESPProgrammer programmer(KBox.getNeopixels(), Serial, Serial1);
  while (programmer.getState() != ESPProgrammer::ProgrammerState::Done) {
    programmer.loop();
  }
  DEBUG("Exiting programmer mode");
  CPU_RESTART;
}

