/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <Debug.h>
#include "ADCTask.h"
#include "../drivers/board.h"

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

  DEBUG("ADCs: Supply: %i Bat1: %i Bat2: %i Bat3: %i", supply_adc, bat1_adc, bat2_adc, bat3_adc);

  supply = supply_adc * analog_max_voltage / adc.getMaxValue();
  bat1 = bat1_adc * analog_max_voltage / adc.getMaxValue();
  bat2 = bat2_adc * analog_max_voltage / adc.getMaxValue();
  bat3 = bat3_adc * analog_max_voltage / adc.getMaxValue();

  DEBUG("ADC - Supply: %f Bat1: %f Bat2: %f Bat3: %f", supply, bat1, bat2, bat3);
}
