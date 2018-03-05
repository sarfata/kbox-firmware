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
#include <Adafruit_NeoPixel.h>
#include <ADC.h>
#include <Bounce.h>
#include <Encoder.h>
#include <ILI9341_t3.h>
#include <SdFat.h>
#include "board.h"


/* Backlight intensity control */
using BacklightIntensity = uint16_t;
const BacklightIntensity BacklightIntensityMax = 255;
const BacklightIntensity BacklightIntensityOff = 0;

typedef struct { uint32_t dwords[4]; } tKBoxSerialNumber;

/**
 * KBox class to represent all the hardware. This class is statically allocated in
 * @file KBox.cpp */
class KBoxHardware {
  private:
    Adafruit_NeoPixel neopixels = Adafruit_NeoPixel(2, neopixel_pin, NEO_GRB + NEO_KHZ800);
    Encoder encoder = Encoder(encoder_a, encoder_b);
    Bounce button = Bounce(encoder_button, 10 /* ms */);
    ILI9341_t3 display = ILI9341_t3(display_cs, display_dc, 255 /* rst unused */, display_mosi, display_sck, display_miso);
    ADC adc;
    #if defined(__MK66FX1M0__)
      // SDIO support for Builtin SD-Card in Teensy 3.6
      SdFatSdio _sd;
    #else
      // KBox, Teensy 3.2
      SdFat _sd;
    #endif

    bool _sdCardSuccess = false;

    bool sdCardInit();

  public:
    void setup();

    ILI9341_t3& getDisplay() {
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

    #if defined(__MK66FX1M0__)
      // SDIO support for Builtin SD-Card in Teensy 3.6
      SdFatSdio& getSdFat() {
        return _sd;
      };
    #else
      // KBox, Teensy 3.2
      SdFat& getSdFat() {
        return _sd;
      };
    #endif

    bool isSdCardUsable() const {
      return _sdCardSuccess;
    };

    void setBacklight(BacklightIntensity intensity);

    void espInit();
    void espRebootInFlasher();
    void espRebootInProgram();

    /**
     * Immediately reboots KBox.
     */
    void rebootKBox();

    /**
     * Read KBox serial number and return it.
     */
    void readKBoxSerialNumber(tKBoxSerialNumber &sn);
};

extern KBoxHardware KBox;
