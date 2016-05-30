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
#include <Adafruit_BMP280.h>
#include "MfgTest.h"

class BarometerTest : public MfgTest {
  private:
    Adafruit_BMP280 bmp280;
  public:
    BarometerTest(KBox& kbox) : MfgTest(kbox, "BarometerTest", 3000) {};

    void setup() {
      if (!bmp280.begin(bmp280_address)) {
        fail("Error initializing bmp280");
      }
      setInstructions("Testing barometer...");
    };

    void loop() {
      float temperature = bmp280.readTemperature();
      float pressure = bmp280.readPressure();

      setMessage("Temperature: " + String(temperature) + "C Pressure: " + String(pressure / 100) + "hPa");
      if (temperature > 10 && temperature < 50 && pressure > 90000 && pressure < 130000) {
        pass();
      }
    };
};
