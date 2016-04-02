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

  // Create all the receiver task first
  WiFiTask *wifi = new WiFiTask();

  // Create all the generating tasks and connect them

  NMEA2000Task *n2kTask = new NMEA2000Task();
  n2kTask->connectTo(*wifi);

  ADCTask *adcTask = new ADCTask();

  // Convert battery measurement into n2k messages before sending them to wifi.
  VoltageN2kConverter *voltageConverter = new VoltageN2kConverter();
  adcTask->connectTo(*voltageConverter);
  voltageConverter->connectTo(*wifi);
  voltageConverter->connectTo(*n2kTask);

  NMEAReaderTask *reader1 = new NMEAReaderTask(NMEA1_SERIAL);
  NMEAReaderTask *reader2 = new NMEAReaderTask(NMEA2_SERIAL);
  reader1->connectTo(*wifi);
  //reader2->connectTo(*wifi);

  IMUTask *imuTask = new IMUTask();
  //imuTask->connectTo(*wifi);

  BarometerTask *baroTask = new BarometerTask();

  BarometerN2kConverter *bn2k = new BarometerN2kConverter();
  bn2k->connectTo(*wifi);
  bn2k->connectTo(*n2kTask);

  baroTask->connectTo(*bn2k);

  // Add all the tasks
  kbox.addTask(new IntervalTask(new RunningLightTask(), 250));
  kbox.addTask(new IntervalTask(adcTask, 1000));
  kbox.addTask(new IntervalTask(imuTask, 50));
  kbox.addTask(new IntervalTask(baroTask, 1000));
  kbox.addTask(n2kTask);
  kbox.addTask(reader1);
  kbox.addTask(reader2);
  kbox.addTask(wifi);

  //EncoderTestPage *encPage = new EncoderTestPage();
  //mfd->addPage(encPage);

  //ShuntMonitorPage *shuntPage = new ShuntMonitorPage();
  //mfd->addPage(shuntPage);

  //SdcardTestPage *sdcardPage = new SdcardTestPage();
  //mfd->addPage(sdcardPage);

  kbox.addPage(new ClockPage());

  kbox.setup();
  DEBUG("setup done");
}

void loop() {
  kbox.loop();
}
