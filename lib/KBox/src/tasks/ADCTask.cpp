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

bool useRudderSensor = true; //varaible used to determine whether bat3 input used for RudderSensor or battery input

void ADCTask::loop() {
  int supply_adc = adc.analogRead(supply_analog, ADC_0);
  int bat1_adc = adc.analogRead(bat1_analog, ADC_0);
  int bat2_adc = adc.analogRead(bat2_analog, ADC_0);
  //int bat3_adc = adc.analogRead(bat3_analog, ADC_0);
    
  supply = supply_adc * analog_max_voltage / adc.getMaxValue();
  bat1 = bat1_adc * analog_max_voltage / adc.getMaxValue();
  bat2 = bat2_adc * analog_max_voltage / adc.getMaxValue();
    
  VoltageMeasurement m1(0, "house", bat1);
  VoltageMeasurement m2(1, "starter", bat2);
  VoltageMeasurement mSupply(3, "supply", supply);
    
  sendMessage(m1);
  sendMessage(m2);
  sendMessage(mSupply);
    
  if (useRudderSensor == true){
    DEBUG("useRudderSensor is true");
        
    int sensorValue = adc.analogRead(bat3_analog, ADC_0);  //rudder sensor on bat3_analog input
        
    //the max reading based on the potentiometer I am using at 3.3v is 680.  The rudderFactor is intended such that this maxValue
    //translates to 66 which is the assumed rudder swing from lock to lock.  The factor should be adjusted based on the actual max value and the
    //actual rudder swing. The calculation
        
    double maxValue = 680;
    double midAngle = maxValue / 2;
    double rudderFactor = 10.31;
    //use this if max sensorValue is when rudder is fully to port
    //double rawRudderAngle = sensorValue  / rudderFactor; //rawRudderAngle will range from 66 to port, 0 to starboard
    //double rudderDisplayAngle = ((sensorValue - midAngle) / rudderFactor) * -1; //displayRudderAngle will range from -33 to port, +33 to starboard
        
    //use this if max sensorValue is when rudder is fully to starboard
    double rawRudderAngle = ((sensorValue  / rudderFactor) - 66) * -1; //rawRudderAngle will range from 66 to port, 0 to starboard
    double rudderDisplayAngle = ((sensorValue - midAngle) / rudderFactor); //displayRudderAngle will range from -33 to port, +33 to starboard
        
    //DEBUG("sensorValue - %d",sensorValue);
    //DEBUG("rawRudderAngle - %.0f",rawRudderAngle);
    //DEBUG("rudderDisplayAngle - %.0f",rudderDisplayAngle);
        
    RUDMessage rm(rawRudderAngle, rudderDisplayAngle);
    sendMessage(rm);
      
  } else {
        
    int bat3_adc = adc.analogRead(bat3_analog, ADC_0);
    VoltageMeasurement m3(4, "bat3", bat3);
    sendMessage(m3);
  }
    
  //DEBUG("ADC - Supply: %sV Bat1: %sV Bat2: %sV Bat3: %sV", 
      //String(supply, 2).c_str(), String(bat1, 2).c_str(), String(bat2, 2).c_str(), String(bat3, 2).c_str());

}
