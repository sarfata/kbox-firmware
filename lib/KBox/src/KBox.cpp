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
#include "KBox.h"
#include <i2c_t3.h>

void KBox::setup() {
  pinMode(led_pin, OUTPUT);

  // Initialize our I2C bus
  Wire1.begin();
  // BNO055 needs up to 1ms to read 6 registers
  Wire1.setTimeout(5000);
  // BNO055 only supports up to 400kHz
  //Wire1.setRate(I2C_RATE_400);

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
  NMEA1_SERIAL.begin(38400);
  NMEA2_SERIAL.begin(38400);

  // Initialize ADC
  adc.setAveraging(1);
  adc.setResolution(12);
  adc.setConversionSpeed(ADC_LOW_SPEED);
  adc.setSamplingSpeed(ADC_HIGH_SPEED);

  // Do not configure the reset pin. this seems to cause problems.
  //digitalWrite(imu_reset, 1);
  //pinMode(imu_reset, OUTPUT);
  pinMode(imu_int, INPUT);

#ifndef BOARD_v1_revA
  adc.setReference(ADC_REF_EXT, ADC_0);
  adc.setReference(ADC_REF_EXT, ADC_1);
#endif
  // If you do not force the ADC later, make sure to configure ADC1 too because
  // the ADC library might decide to use it (it round-robins read requests).
  // Also, it seems A11 and A14 (bat1 and bat3) can only be read by ADC_0
  // We could optimize reading speed by reading two adcs in parallel.
  //adc.setAveraging(32, ADC_1);
  //adc.setResolution(12, ADC_1);
  //adc.setConversionSpeed(ADC_LOW_SPEED, ADC_1);
  //adc.setSamplingSpeed(ADC_HIGH_SPEED, ADC_1);


  taskManager.addTask(&mfd);
  taskManager.setup();
}

void KBox::loop() {
  taskManager.loop();
}
