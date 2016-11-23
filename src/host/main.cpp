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
#include <SPI.h>
#include <Encoder.h>
#include <i2c_t3.h>
#include <KBox.h>
#include "util/ESPProgrammer.h"

// Those are included here to force platformio to link to them.
// See issue github.com/platformio/platformio/issues/432 for a better way to do this.
#include <Bounce.h>

#include "ClockPage.h"

ILI9341_t3 *display;

void setup() {

  // Reinitialize debug here because in some configurations 
  // (like logging to nmea2 output), the kbox setup might have messed
  // up the debug configuration.
  DEBUG_INIT();
  DEBUG("setup done");

  analogWrite(display_backlight, BacklightIntensityMax);

  display = new ILI9341_t3(display_cs, display_dc, 255 /* rst unused */, display_mosi, display_sck, display_miso);

  display->begin();
  display->fillScreen(ILI9341_BLUE);
  display->setRotation(display_rotation);
}

void loop() {
}
