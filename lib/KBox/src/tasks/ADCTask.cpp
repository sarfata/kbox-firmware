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
#include "ADCTask.h"
#include "KBoxDebug.h"
#include "../drivers/board.h"

#include <NMEA2000.h>
#include <N2kMessages.h>

void ADCTask::loop() {
  int supply_adc = adc.analogRead(supply_analog, ADC_0);
  int bat1_adc = adc.analogRead(bat1_analog, ADC_0);
  int bat2_adc = adc.analogRead(bat2_analog, ADC_0);
  int bat3_adc = adc.analogRead(bat3_analog, ADC_0);
    
  //permits rudder sensor on bat3 input.  Eventually should move rudder sensor calcs to a separate task
  double maxAngle = 680; //this is the max reading based on the potentiometer I am using at 3.3v.
  double midAngle = maxAngle / 2;
  int rudderFactor = 10; //680 / 10 is close to the rudder angle range currently defined as 0-66.  Factor could require a bit of tuning based on the actual installation in the boat
  double rudderAngle = (bat3_adc - midAngle) / rudderFactor;

  supply = supply_adc * analog_max_voltage / adc.getMaxValue();
  bat1 = bat1_adc * analog_max_voltage / adc.getMaxValue();
  bat2 = bat2_adc * analog_max_voltage / adc.getMaxValue();
  //bat3 = bat3_adc * analog_max_voltage / adc.getMaxValue();
   bat3 = rudderAngle;

  //DEBUG("ADC - Supply: %sV Bat1: %sV Bat2: %sV Bat3: %sV", 
      //String(supply, 2).c_str(), String(bat1, 2).c_str(), String(bat2, 2).c_str(), String(bat3, 2).c_str());

  VoltageMeasurement m1(0, "house", bat1);
  VoltageMeasurement m2(1, "starter", bat2);
  VoltageMeasurement mSupply(3, "supply", supply);
  VoltageMeasurement m3(4, "bat3", bat3);

  sendMessage(m1);
  sendMessage(m2);
  sendMessage(m3);
  sendMessage(mSupply);
}
