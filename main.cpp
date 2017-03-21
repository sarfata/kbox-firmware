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
#include <KBox.h>
#include "util/ESPProgrammer.h"

// Those are included here to force platformio to link to them.
// See issue github.com/platformio/platformio/issues/432 for a better way to do this.
#include <Bounce.h>

#include "ClockPage.h"

KBox kbox;

void setup() {
  // Enable float in printf:
  // https://forum.pjrc.com/threads/27827-Float-in-sscanf-on-Teensy-3-1
  asm(".global _printf_float");

  delay(3000);

  DEBUG_INIT();
  DEBUG("Starting");


  digitalWrite(led_pin, 1);

  // Create all the receiver task first
  WiFiTask *wifi = new WiFiTask();

  // Create all the generating tasks and connect them
  NMEA2000Task *n2kTask = new NMEA2000Task();
  n2kTask->connectTo(*wifi);

  ADCTask *adcTask = new ADCTask(kbox.getADC());

  // Convert battery measurement into n2k messages before sending them to wifi.
  VoltageN2kConverter *voltageConverter = new VoltageN2kConverter();
  adcTask->connectTo(*voltageConverter);
  voltageConverter->connectTo(*wifi);
  voltageConverter->connectTo(*n2kTask);

  NMEAReaderTask *reader1 = new NMEAReaderTask(NMEA1_SERIAL);
  NMEAReaderTask *reader2 = new NMEAReaderTask(NMEA2_SERIAL);
  reader1->connectTo(*wifi);
  reader2->connectTo(*wifi);

  IMUTask *imuTask = new IMUTask();
  imuTask->connectTo(*wifi); //RIGM added
  imuTask->connectTo(*n2kTask);

  BarometerTask *baroTask = new BarometerTask();

  BarometerN2kConverter *bn2k = new BarometerN2kConverter();
  bn2k->connectTo(*wifi);
  bn2k->connectTo(*n2kTask);

  baroTask->connectTo(*bn2k);

  SDCardTask *sdcardTask = new SDCardTask();
  reader1->connectTo(*sdcardTask);
  reader2->connectTo(*sdcardTask);
  adcTask->connectTo(*sdcardTask);
  n2kTask->connectTo(*sdcardTask);
  baroTask->connectTo(*sdcardTask);
  imuTask->connectTo(*sdcardTask);
    
  AutoPilotTask *autoPilotTask = new AutoPilotTask();//RIGM added
  autoPilotTask->connectTo(*wifi);
  autoPilotTask->connectTo(*n2kTask);

  // Add all the tasks
  kbox.addTask(new IntervalTask(new RunningLightTask(), 250));
  kbox.addTask(new IntervalTask(adcTask, 1000));
  kbox.addTask(new IntervalTask(imuTask, 50));
  kbox.addTask(new IntervalTask(baroTask, 1000));
  kbox.addTask(n2kTask);
  kbox.addTask(reader1);
  kbox.addTask(reader2);
  kbox.addTask(wifi);
  kbox.addTask(sdcardTask);
  kbox.addTask(autoPilotTask);
    
  NavigationPage *navPage = new NavigationPage(); //RIGM added
  imuTask->connectTo(*navPage);
  adcTask->connectTo(*navPage);
  autoPilotTask->connectTo(*navPage);
  kbox.addPage(navPage);
  navPage->connectTo(*autoPilotTask);

  BatteryMonitorPage *batPage = new BatteryMonitorPage();
  adcTask->connectTo(*batPage);
  kbox.addPage(batPage);

  StatsPage *statsPage = new StatsPage();
  statsPage->setNmea1Task(reader1);
  statsPage->setNmea2Task(reader2);
  statsPage->setNMEA2000Task(n2kTask);
  statsPage->setTaskManager(&(kbox.getTaskManager()));
  statsPage->setSDCardTask(sdcardTask);

  kbox.addPage(statsPage);

  kbox.setup();

  // Reinitialize debug here because in some configurations 
  // (like logging to nmea2 output), the kbox setup might have messed
  // up the debug configuration.
  DEBUG_INIT();
  DEBUG("setup done");

  Serial.setTimeout(0);
  Serial1.setTimeout(0);
  Serial2.setTimeout(0);
  Serial3.setTimeout(0);
}

void loop() {
  if (Serial.baud() == 230400) {
    DEBUG("Entering programmer mode");
    ESPProgrammer programmer(kbox.getNeopixels(), Serial, Serial1);
    while (programmer.getState() != ESPProgrammer::ProgrammerState::Done) {
      programmer.loop();
    }
    DEBUG("Exiting programmer mode");
    CPU_RESTART;
  }
  else {
    kbox.loop();
  }
}
