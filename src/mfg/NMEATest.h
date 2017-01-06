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

#include <KBox.h>
#include <Arduino.h>
#include "MfgTest.h"

class NMEATest : public MfgTest {
  private:
    HardwareSerial& output;
    HardwareSerial& input;

    uint8_t *data;
    size_t dataLength = 2048;
    size_t readIndex = 0;
    size_t writeIndex = 0;

  public:
    NMEATest(String name, HardwareSerial& out, HardwareSerial& in) : MfgTest(name, 3000), output(out), input(in) {};

    void setup() {
      setInstructions("Testing NMEA ports");
      output.begin(38400);
      input.begin(38400);

      if (&output == &NMEA1_SERIAL) {
        Serial.println("Enabling NMEA1 output");
        digitalWrite(nmea1_out_enable, 1);
      }
      else if (&output == &NMEA2_SERIAL) {
        Serial.println("Enabling NMEA2 output");
        digitalWrite(nmea2_out_enable, 1);
      }

      data = (uint8_t*) malloc(dataLength);
      for (uint32_t i = 0; i < dataLength; i++) {
        data[i] = i % 0x100;
      }
    };

    void teardown() {
      free(data);
      data = 0;

      if (output == NMEA1_SERIAL) {
        digitalWrite(nmea1_out_enable, 0);
      }
      else if (output == NMEA2_SERIAL) {
        digitalWrite(nmea2_out_enable, 0);
      }

    };

    void loop() {
      while (writeIndex < dataLength && output.availableForWrite()) {
        output.write(data[writeIndex]);
        writeIndex++;
        setMessage("Sent " + String(writeIndex) + " Recv " + String(readIndex) + " Total " + String(dataLength));
      }

      while (input.available()) {
        uint8_t b = input.read();
        if (b == data[readIndex]) {
          readIndex++;
        }
        else {
          fail("Byte %i should be 0x" + String(data[readIndex]) + " and was 0x" + String(b) + " instead.");
        }
      }

      if (readIndex == dataLength) {
        pass();
      }
    };
};
