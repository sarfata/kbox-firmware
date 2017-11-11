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

#include <i2c_t3.h>
#include "KBoxHardware.h"

KBoxHardware KBox;

void KBoxHardware::setup() {
  pinMode(led_pin, OUTPUT);

  // Initialize our I2C bus
  Wire1.begin();
  // BNO055 needs up to 1ms to read 6 registers
  Wire1.setTimeout(5000);
  //Wire1.setOpMode(I2C_OP_MODE_IMM);
  // BNO055 only supports up to 400kHz
  Wire1.setRate(I2C_RATE_400);

  // Initialize encoder pins
  pinMode(encoder_button, INPUT_PULLUP);

  // Initialize neopixels
  neopixels.begin();
  neopixels.show();
  // seems useful to make sure the neopixels are cleared every boot.
  neopixels.show();

  // Initialize serialports
  digitalWrite(nmea1_out_enable, 0);
  digitalWrite(nmea2_out_enable, 0);
  pinMode(nmea1_out_enable, OUTPUT);
  pinMode(nmea2_out_enable, OUTPUT);
  //NMEA1_SERIAL.begin(38400);
  //NMEA2_SERIAL.begin(38400);

  // Initialize ADC
  adc.setAveraging(1);
  adc.setResolution(12);
  adc.setConversionSpeed(ADC_CONVERSION_SPEED::LOW_SPEED);
  adc.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);

#ifndef BOARD_v1_revA
  adc.setReference(ADC_REFERENCE::REF_EXT, ADC_0);
  adc.setReference(ADC_REFERENCE::REF_EXT, ADC_1);
#endif
  // If you do not force the ADC later, make sure to configure ADC1 too because
  // the ADC library might decide to use it (it round-robins read requests).
  // Also, it seems A11 and A14 (bat1 and bat3) can only be read by ADC_0
  // We could optimize reading speed by reading two adcs in parallel.
  //adc.setAveraging(32, ADC_1);
  //adc.setResolution(12, ADC_1);
  //adc.setConversionSpeed(ADC_LOW_SPEED, ADC_1);
  //adc.setSamplingSpeed(ADC_HIGH_SPEED, ADC_1);

  display.begin();
  display.fillScreen(ILI9341_BLUE);
  display.setRotation(display_rotation);

  setBacklight(BacklightIntensityMax);
}

void KBoxHardware::setBacklight(BacklightIntensity intensity) {
  if (display_backlight) {
    if (intensity > 0) {
      analogWrite(display_backlight, intensity);
    }
    else {
      analogWrite(display_backlight, 0);
    }
  }
}

void KBoxHardware::espInit() {
  WiFiSerial.begin(115200);
  WiFiSerial.setTimeout(0);

  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_rst, 0);

  // wifi_program is gpio0, wifi_cs is gpio15
  // gpio2 is pulled up in hardware
  digitalWrite(wifi_program, 0);
  digitalWrite(wifi_cs, 0);

  pinMode(wifi_enable, OUTPUT);
  pinMode(wifi_rst, OUTPUT);
  pinMode(wifi_program, OUTPUT);

  if (wifi_cs > 0) {
    digitalWrite(wifi_cs, 0);
    pinMode(wifi_cs, OUTPUT);
  }

}

void KBoxHardware::espRebootInFlasher() {
  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_rst, 0);
  digitalWrite(wifi_program, 0);

  delay(10);

  // Boot the ESP module
  digitalWrite(wifi_enable, 1);
  digitalWrite(wifi_rst, 1);
}

void KBoxHardware::espRebootInProgram() {
  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_rst, 0);
  digitalWrite(wifi_program, 1);

  delay(10);

  // Boot the ESP module
  digitalWrite(wifi_enable, 1);
  digitalWrite(wifi_rst, 1);
}
