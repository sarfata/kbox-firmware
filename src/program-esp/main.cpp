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

#include <Arduino.h>
#include <KBoxHardware.h>
#include <KBoxLogging.h>
#include <KBoxLoggerStream.h>
#include "comms/SlipStream.h"
#include "../host/esp-programmer/ESPProgrammer.h"

class ESPProgrammerDelegateImpl : public ESPProgrammerDelegate {
  void rebootInFlasher() {
    KBox.espRebootInFlasher();
  };
};

ESPProgrammerDelegateImpl delegate;

ESPProgrammer programmer(KBox.getNeopixels(), Serial, Serial1, delegate);

void setup() {
  KBox.setup();

  // Use Serial3 as the Debug output to avoid trashing the communication
  // with the programmer program running on host.
  digitalWrite(nmea2_out_enable, 1);
  pinMode(nmea2_out_enable, OUTPUT);
  Serial3.begin(920800);
  KBoxLogging.setLogger(new KBoxLoggerStream(Serial3));


  DEBUG("Starting esp program");
  KBox.espInit();
  KBox.espRebootInProgram();

  Serial.setTimeout(10000);
  Serial1.setTimeout(10000);
  Serial3.setTimeout(10000);
}

void loop() {
  programmer.loop();
}

