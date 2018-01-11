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
#include "common/signalk/SKHub.h"
#include "host/config/KBoxConfig.h"
#include "host/config/KBoxConfigParser.h"
#include "host/drivers/ILI9341GC.h"
#include "host/pages/BatteryMonitorPage.h"
#include "host/pages/StatsPage.h"
#include "host/services/MFD.h"
#include "host/services/ADCService.h"
#include "host/services/BarometerService.h"
#include "host/services/IMUService.h"
#include "host/pages/IMUMonitorPage.h"
#include "host/services/NMEA2000Service.h"
#include "host/services/SerialService.h"
#include "host/services/RunningLightService.h"
#include "host/services/SDCardTask.h"
#include "host/services/USBService.h"
#include "host/services/WiFiService.h"
#include "host/services/AnalogSensorService.h"

static const char *configFilename = "kbox-config.json";

ILI9341GC gc(KBox.getDisplay(), Size(320, 240));
MFD mfd(gc, KBox.getEncoder(), KBox.getButton());
TaskManager taskManager;
SKHub skHub;
KBoxConfig config;

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

  // Load configuration if available
  KBoxConfigParser configParser;
  configParser.defaultConfig(config);
  if (KBox.getSdFat().exists(configFilename)) {
    File configFile = KBox.getSdFat().open(configFilename);
    // We can afford to allocate a lot of memory on the stack for this because we have not started doing
    // anything real yet.
    StaticJsonBuffer<4096> jsonBuffer;
    JsonObject &root =jsonBuffer.parseObject(configFile);

    if (root.success()) {
      DEBUG("Loading configuration from SDCard");
      configParser.parseKBoxConfig(root, config);
    }
    else {
      ERROR("Failed to parse configuration file");
    }
  }
  else {
    DEBUG("No configuration file found. Using defaults.");
  }


  // Instantiate all our services
  WiFiService *wifi = new WiFiService(config.wifiConfig, skHub, gc);

  ADCService *adcService = new ADCService(skHub, KBox.getADC());
  BarometerService *baroService = new BarometerService(skHub);
  IMUService *imuService = new IMUService(config.imuConfig, skHub);

  NMEA2000Service *n2kService = new NMEA2000Service(config.nmea2000Config,
                                                    skHub);
  n2kService->connectTo(*wifi);

  SerialService *reader1 = new SerialService(config.serial1Config, skHub, NMEA1_SERIAL);
  SerialService *reader2 = new SerialService(config.serial2Config, skHub, NMEA2_SERIAL);
  reader1->connectTo(*wifi);
  reader2->connectTo(*wifi);

  SDCardTask *sdcardTask = new SDCardTask();
  reader1->connectTo(*sdcardTask);
  reader2->connectTo(*sdcardTask);
  n2kService->connectTo(*sdcardTask);

  AnalogSensorService* analogSensorService =
    new AnalogSensorService(config.analogSensorConfig, skHub);

  // Add all the tasks
  taskManager.addTask(&mfd);
  taskManager.addTask(new IntervalTask(new RunningLightService(), 250));
  taskManager.addTask(new IntervalTask(adcService, 1000));
  if (config.imuConfig.enabled) {
    taskManager.addTask(new IntervalTask(imuService, 1000 / config.imuConfig.frequency));
  }
  if (config.barometerConfig.enabled) {
    taskManager.addTask(new IntervalTask(baroService, 1000 / config.barometerConfig.frequency));
  }
  if (config.analogSensorConfig.enabled) {
    taskManager.addTask(
      new IntervalTask(analogSensorService,
                       1000 / config.analogSensorConfig.frequency));

  }
  taskManager.addTask(n2kService);
  taskManager.addTask(reader1);
  taskManager.addTask(reader2);
  taskManager.addTask(wifi);
  taskManager.addTask(sdcardTask);
  taskManager.addTask(&usbService);

  BatteryMonitorPage *batPage = new BatteryMonitorPage(skHub);
  mfd.addPage(batPage);

  if (config.imuConfig.enabled) {
    // At the moment the IMUMonitorPage is working with built-in sensor only
    IMUMonitorPage *imuPage = new IMUMonitorPage(config.imuConfig, skHub, *imuService);
    mfd.addPage(imuPage);
  }

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
