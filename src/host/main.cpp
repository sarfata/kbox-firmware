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
#include <Bounce.h>
#include <Adafruit_NeoPixel.h>
#include <i2c_t3.h>
#include <Debug.h>
#include <KBox.h>

// Those are included here to force platformio to link to them. 
// See issue github.com/platformio/platformio/issues/432 for a better way to do this.
#include <ili9341display.h>
#include <Bounce.h>

#include "EncoderTestPage.h"
#include "WifiTestPage.h"
#include "ShuntMonitorPage.h"
#include "NMEA2000Page.h"
#include "NMEAPage.h"
#include "IMUPage.h"
#include "ADCPage.h"
#include "SdcardTestPage.h"
#include "ClockPage.h"

#define LED_PIN 13

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, neopixel_pin, NEO_GRB + NEO_KHZ800);

MFD *mfd;
TaskManager taskManager;

void setup() {
  delay(3000);

  DEBUG_INIT();
  DEBUG("Starting");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 1);
  DEBUG("led ok");

  // Initialize our I2C bus
  //Wire1.begin();
  // BNO055 needs up to 1ms to read 6 registers
  //Wire1.setTimeout(5000);
  //Wire1.setOpMode(I2C_OP_MODE_IMM);

  DEBUG("wire ok");

  //taskManager.addTask(new IntervalTask(new BarometerTask(), 1000));
  taskManager.setup();

  DEBUG("task manager ok");
  ILI9341Display *display = new ILI9341Display();
  DEBUG("display ok");
  Encoder *encoder = new Encoder(encoder_a, encoder_b);
  pinMode(encoder_button, INPUT_PULLUP);
  Bounce *button = new Bounce(encoder_button, 10 /* ms */);

  DEBUG("Starting mfd");
  mfd = new MFD(*display, *encoder, *button);

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

  ClockPage *clockPage = new ClockPage();
  mfd->addPage(clockPage);

  strip.begin();
  strip.show();

  DEBUG("setup done");
}

void loop() {
  taskManager.loop();
  mfd->loop();
}
