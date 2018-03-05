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

#include <KBoxLogging.h>
#include <KBoxHardware.h>

#include "common/signalk/SKUpdateStatic.h"

#include "ADCService.h"

void ADCService::loop() {
  int supply_adc = _adc.analogRead(supply_analog, ADC_0);
  int adc1_adc = _adc.analogRead(adc1_analog, ADC_0);
  int adc2_adc = _adc.analogRead(adc2_analog, ADC_0);
  int adc3_adc = _adc.analogRead(adc3_analog, ADC_0);

  _supply = supply_adc * analog_max_voltage / _adc.getMaxValue();
  _adc1 = adc1_adc * analog_max_voltage / _adc.getMaxValue();
  _adc2 = adc2_adc * analog_max_voltage / _adc.getMaxValue();
  _adc3 = adc3_adc * analog_max_voltage / _adc.getMaxValue();

  // FIXME: We should have configuration options to describe what each input is
  // connected to instead of hard-coding names.
  SKUpdateStatic<4> sk;
  sk.setSource(SKSource::sourceForKBoxSensor(SKSourceInputKBoxADC));
  sk.setElectricalBatteriesVoltage("engine", _adc1);
  sk.setElectricalBatteriesVoltage("house", _adc2);
  sk.setElectricalBatteriesVoltage("dc3", _adc3);
  sk.setElectricalBatteriesVoltage("kbox-supply", _supply);

  _skHub.publish(sk);
}
