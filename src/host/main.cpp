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

#include <SPI.h>
#include <Encoder.h>
#include <i2c_t3.h>
#include <Debug.h>
#include <KBox.h>

// Those are included here to force platformio to link to them. 
// See issue github.com/platformio/platformio/issues/432 for a better way to do this.
#include <ili9341display.h>
#include <Bounce.h>

#include "ClockPage.h"

KBox kbox;

void setup() {
  delay(3000);

  DEBUG_INIT();
  DEBUG("Starting");


  digitalWrite(led_pin, 1);


  kbox.addTask(new IntervalTask(new RunningLightTask(), 250));
  kbox.addTask(new IntervalTask(new ADCTask(), 1000));
  kbox.addTask(new NMEA2000Task());

  //taskManager.addTask(new IntervalTask(new BarometerTask(), 1000));

  //EncoderTestPage *encPage = new EncoderTestPage();
  //mfd->addPage(encPage);

  //WifiTestPage *wifiPage = new WifiTestPage();
  //mfd->addPage(wifiPage);

  //ShuntMonitorPage *shuntPage = new ShuntMonitorPage();
  //mfd->addPage(shuntPage);

  //BarometerPage *barometerPage = new BarometerPage();
  //mfd->addPage(barometerPage);

  //IMUPage *imuPage = new IMUPage();
  //mfd->addPage(imuPage);

  //NMEA2000Page *nmea2000Page = new NMEA2000Page();
  //mfd->addPage(nmea2000Page);

  //NMEAPage *nmeaPage = new NMEAPage();
  //mfd->addPage(nmeaPage);

  //ADCPage *adcPage = new ADCPage();
  //mfd->addPage(adcPage);
  
  //SdcardTestPage *sdcardPage = new SdcardTestPage();
  //mfd->addPage(sdcardPage);

  kbox.addPage(new ClockPage());

  kbox.setup();
  DEBUG("setup done");
}

void loop() {
  kbox.loop();
}
