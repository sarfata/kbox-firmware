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
