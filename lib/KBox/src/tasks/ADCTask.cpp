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
#include <Debug.h>
#include "ADCTask.h"
#include "../drivers/board.h"

#include <NMEA2000.h>
#include <N2kMessages.h>

void ADCTask::setup() {
  DEBUG("ADCTask setup");
  adc.setAveraging(1);
  adc.setResolution(12);
  adc.setConversionSpeed(ADC_LOW_SPEED);
  adc.setSamplingSpeed(ADC_HIGH_SPEED);

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
}

void ADCTask::loop() {
  int supply_adc = adc.analogRead(supply_analog, ADC_0);
  int bat1_adc = adc.analogRead(bat1_analog, ADC_0);
  int bat2_adc = adc.analogRead(bat2_analog, ADC_0);
  int bat3_adc = adc.analogRead(bat3_analog, ADC_0);

  //DEBUG("ADCs: Supply: %i Bat1: %i Bat2: %i Bat3: %i", supply_adc, bat1_adc, bat2_adc, bat3_adc);

  supply = supply_adc * analog_max_voltage / adc.getMaxValue();
  bat1 = bat1_adc * analog_max_voltage / adc.getMaxValue();
  bat2 = bat2_adc * analog_max_voltage / adc.getMaxValue();
  bat3 = bat3_adc * analog_max_voltage / adc.getMaxValue();

  //DEBUG("ADC - Supply: %.2fV Bat1: %.2fV Bat2: %.2fV Bat3: %.2fV", supply, bat1, bat2, bat3);

  // Would be nice if the indexes were configurable.
  // In the future, we will have:
  //  0: INA191 measured service battery voltage (+current)
  //  1: nmea2000 supply
  //  2/3/4: adc inputs - We could re-label them on the board
  VoltageMeasurement mSupply(1, "supply", supply);
  VoltageMeasurement m1(2, "house", bat1);
  VoltageMeasurement m2(3, "starter", bat2);
  VoltageMeasurement m3(4, "bat3", bat3);

  sendMessage(m1);
  sendMessage(m2);
  sendMessage(m3);
  sendMessage(mSupply);
}
