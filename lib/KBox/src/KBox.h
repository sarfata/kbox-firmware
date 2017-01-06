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

#include <KBoxLogging.h>
#include "MFD.h"
#include "util/TaskManager.h"
#include "KMessage.h"


/* Drivers */
#include "drivers/board.h"
#include "drivers/ili9341display.h"
#include "drivers/esp8266.h"
#include <ADC.h>

/* Services (was Tasks) */
#include "services/USBService.h"
#include "tasks/RunningLightTask.h"
#include "tasks/BarometerTask.h"
#include "tasks/ADCTask.h"
#include "tasks/NMEA2000Task.h"
#include "tasks/IMUTask.h"
#include "tasks/NMEAReaderTask.h"
#include "tasks/WiFiTask.h"

/* Converters */
#include "converters/VoltageN2kConverter.h"
#include "converters/BarometerN2kConverter.h"

/* Pages */
#include "pages/BatteryMonitorPage.h"
#include "pages/StatsPage.h"

/* Other dependencies */
#include <Adafruit_NeoPixel.h>
#include <Bounce.h>
#include <font_DroidSans.h>

/**
 * KBox class to represent all the hardware. This class is statically allocated in
 * @file KBox.cpp */
class KBoxClass {
  private:
    Adafruit_NeoPixel neopixels = Adafruit_NeoPixel(2, neopixel_pin, NEO_GRB + NEO_KHZ800);
    ILI9341Display display = ILI9341Display();
    Encoder encoder = Encoder(encoder_a, encoder_b);
    Bounce button = Bounce(encoder_button, 10 /* ms */);
    ADC adc;

  public:
    KBoxClass();

    Display& getDisplay() {
      return display;
    };

    Encoder& getEncoder() {
      return encoder;
    };

    Bounce& getButton() {
      return button;
    };

    Adafruit_NeoPixel& getNeopixels() {
      return neopixels;
    };

    ADC& getADC() {
      return adc;
    };
};

extern KBoxClass KBox;
