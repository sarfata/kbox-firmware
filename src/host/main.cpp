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

#include <KBoxHardware.h>
#include "common/os/TaskManager.h"
#include "common/os/Task.h"
#include "host/drivers/ILI9341GC.h"
#include "host/pages/BatteryMonitorPage.h"
#include "host/pages/StatsPage.h"
#include "host/services/ADCTask.h"
#include "host/services/BarometerN2kConverter.h"
#include "host/services/BarometerTask.h"
#include "host/services/IMUTask.h"
#include "host/services/MFD.h"
#include "host/services/NMEA2000Service.h"
#include "host/services/NMEAService.h"
#include "host/services/RunningLightTask.h"
#include "host/services/SDCardTask.h"
#include "host/services/USBService.h"
#include "host/services/VoltageN2kConverter.h"
#include "host/services/WiFiService.h"

ILI9341GC gc(KBox.getDisplay(), Size(320, 240));
MFD mfd(gc, KBox.getEncoder(), KBox.getButton());
TaskManager taskManager;
SKHub skHub;

USBService usbService(gc);

void setup() {
  // Enable float in printf:
  // https://forum.pjrc.com/threads/27827-Float-in-sscanf-on-Teensy-3-1
  asm(".global _printf_float");

  KBox.setup();

  // Clears the screen
  mfd.setup();

  delay(1000);

  Serial.begin(115200);
  KBoxLogging.setLogger(&usbService);

  DEBUG("Starting");

  digitalWrite(led_pin, 1);

  // Create all the receiver task first
  WiFiService *wifi = new WiFiService(gc);

  // Create all the generating tasks and connect them
  NMEA2000Service *n2kService = new NMEA2000Service(skHub);
  n2kService->connectTo(*wifi);

  ADCTask *adcTask = new ADCTask(KBox.getADC());

  // Convert battery measurement into n2k messages before sending them to wifi.
  VoltageN2kConverter *voltageConverter = new VoltageN2kConverter();
  adcTask->connectTo(*voltageConverter);
  voltageConverter->connectTo(*wifi);
  voltageConverter->connectTo(*n2kService);

  NMEAService *reader1 = new NMEAService(skHub, NMEA1_SERIAL);
  NMEAService *reader2 = new NMEAService(skHub, NMEA2_SERIAL);
  reader1->connectTo(*wifi);
  reader2->connectTo(*wifi);

  IMUTask *imuTask = new IMUTask();
  imuTask->connectTo(*wifi);
  imuTask->connectTo(*n2kService);

  BarometerTask *baroTask = new BarometerTask();

  BarometerN2kConverter *bn2k = new BarometerN2kConverter();
  bn2k->connectTo(*wifi);
  bn2k->connectTo(*n2kService);

  baroTask->connectTo(*bn2k);

  SDCardTask *sdcardTask = new SDCardTask();
  reader1->connectTo(*sdcardTask);
  reader2->connectTo(*sdcardTask);
  adcTask->connectTo(*sdcardTask);
  n2kService->connectTo(*sdcardTask);
  baroTask->connectTo(*sdcardTask);
  imuTask->connectTo(*sdcardTask);


  // Add all the tasks
  taskManager.addTask(&mfd);
  taskManager.addTask(new IntervalTask(new RunningLightTask(), 250));
  taskManager.addTask(new IntervalTask(adcTask, 1000));
  taskManager.addTask(new IntervalTask(imuTask, 50));
  taskManager.addTask(new IntervalTask(baroTask, 1000));
  taskManager.addTask(n2kService);
  taskManager.addTask(reader1);
  taskManager.addTask(reader2);
  taskManager.addTask(wifi);
  taskManager.addTask(sdcardTask);
  taskManager.addTask(&usbService);

  BatteryMonitorPage *batPage = new BatteryMonitorPage();
  adcTask->connectTo(*batPage);
  mfd.addPage(batPage);

  StatsPage *statsPage = new StatsPage();
  statsPage->setSDCardTask(sdcardTask);

  mfd.addPage(statsPage);

  taskManager.setup();

  // Reinitialize debug here because in some configurations
  // (like logging to nmea2 output), the kbox setup might have messed
  // up the debug configuration.
  DEBUG("setup done");

  Serial.setTimeout(0);
  Serial1.setTimeout(0);
  Serial2.setTimeout(0);
  Serial3.setTimeout(0);
}

void loop() {
  taskManager.loop();
}
