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
#include <Adafruit_INA219.h>
#include "MfgTest.h"

class ShuntTest : public MfgTest {
  private:
    Adafruit_INA219 ina219;
  public:
    ShuntTest(KBox& kbox) : MfgTest(kbox, "ShuntTest", 30000) {};

    void setup() {
      ina219.begin(ina219_address);
      setInstructions("Connect ShuntBat to Load - Apply 12V");
    };

    void loop() {
      float shuntVoltage = ina219.getShuntVoltage_mV();
      float busVoltage = ina219.getBusVoltage_V();
      float current = ina219.getCurrent_mA();

      setMessage("Bus: " + String(busVoltage, 2) + "V Shunt: " + String(shuntVoltage, 2) + "mV Curent: " + String(current) + "mA");

      if (busVoltage > 11.9 && busVoltage < 12.1) {
        if (shuntVoltage < 500) {
          pass();
        }
      }
    };
};

