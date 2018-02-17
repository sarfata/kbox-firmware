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
#include <Seasmart.h>
#include "common/comms/Kommand.h"
#include "common/stats/KBoxMetrics.h"
#include "common/signalk/SKNMEAConverter.h"
#include "common/signalk/SKNMEAConverterConfig.h"
#include "../esp-programmer/ESPProgrammer.h"
#include "../drivers/ILI9341GC.h"

#include "USBService.h"


USBService::USBService(GC &gc, SKHub &hub) : Task("USBService"), _slip(Serial, 2048),
                                 _streamLogger(KBoxLoggerStream(Serial)),
                                 _skHub(hub),
                                 _pingHandler(), _screenshotHandler(gc),
                                 _state(ConnectedDebug) {

}

void USBService::setup() {
  Serial.setTimeout(0);
  _skHub.subscribe(this);
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
    case ConnectedNMEAInterface:
      /* Discard all logs when in NMEA interface mode. */
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

  /* Switching serial port to 38400 on computer signals that the host
   * wants to just receive NMEA sentences and no debug messages.
   */
  if (Serial.baud() == 38400) {
    _state = ConnectedNMEAInterface;
  }

  switch (_state) {
    case ConnectedDebug:
      loopNMEAInterfaceMode();
      break;
    case ConnectedFrame:
      loopConnectedFrame();
      break;
    case ConnectedESPProgramming:
      loopConnectedESPProgramming();
      break;
    case ConnectedNMEAInterface:
      loopNMEAInterfaceMode();
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

    KommandReader kr = KommandReader(frame, len);

    KommandHandler *handlers[] = { &_pingHandler, &_screenshotHandler,
                                   &_fileReadHandler, &_fileWriteHandler,
                                   &_rebootHandler,
                                   nullptr };
    if (KommandHandler::handleKommandWithHandlers(handlers, kr, _slip)) {
      KBoxMetrics.event(KBoxEventUSBValidKommand);
    }
    else {
      KBoxMetrics.event(KBoxEventUSBInvalidKommand);
    }

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
  KBox.rebootKBox();
}

/**
 * nothing yet. in the future we will read data sent by computer.
 */
void USBService::loopNMEAInterfaceMode() {
  char buf[1024];

  if (Serial.available()) {
    /* int bytesRead = */Serial.readBytes(buf, sizeof(buf));

    /* discard data for now */
  }
}

/**
 * Sends a frame with a logging message.
 */
void USBService::sendLogFrame(KBoxLoggingLevel level, const char *fname, int lineno, const char *fmt, va_list fmtargs) {
  FixedSizeKommand<MaxLogFrameSize> logKmd(KommandLog);
  logKmd.append16(level);
  logKmd.append16(lineno);
  logKmd.append16(strlen(fname));
  logKmd.appendNullTerminatedString(fname);

  uint8_t *bytes;
  size_t *index;

  logKmd.captureBuffer(&bytes, &index);

  *index += vsnprintf((char*)bytes + *index, MaxLogFrameSize - *index, fmt, fmtargs);
  (*index)++; // for the null-terminating byte

  _slip.writeFrame(bytes, *index);
}

void USBService::updateReceived(const SKUpdate& u) {
  /* This is where we convert the data in SignalK format that floats inside KBox
   * to messages that will be sent to the computer over USB in NMEA interface mode.
   */

  if (_state == ConnectedNMEAInterface) {
    SKNMEAConverterConfig config;
    SKNMEAConverter nmeaConverter(config);
    // The converter will call this->write(NMEASentence) for every generated sentence
    nmeaConverter.convert(u, *this);
  }
}

bool USBService::write(const SKNMEASentence &nmeaSentence) {
  if (_state != ConnectedNMEAInterface) {
    return true;
  }

  Serial.println(nmeaSentence.c_str());
  return true;
}

bool USBService::write(const tN2kMsg &msg) {
  if (_state != ConnectedNMEAInterface) {
    return true;
  }

  if (msg.DataLen > 500) {
    return false;
  }

  char pcdin[30 + msg.DataLen * 2];
  if (N2kToSeasmart(msg, millis(), pcdin, sizeof(pcdin)) < sizeof(pcdin)) {
    Serial.println(pcdin);
    return true;
  } else {
    return false;
  }
}

