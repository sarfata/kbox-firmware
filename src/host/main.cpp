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
#include <MFD.h>
#include <ili9341display.h>
#include <Debug.h>
#include "EncoderTestPage.h"
#include "WifiTestPage.h"
#include "ShuntMonitorPage.h"
#include "BarometerPage.h"
#include "NMEA2000Page.h"
#include "NMEAPage.h"
#include "IMUPage.h"
#include "ADCPage.h"
#include "SdcardTestPage.h"
#include "ClockPage.h"
#include "board.h"

#define LED_PIN 13

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, neopixel_pin, NEO_GRB + NEO_KHZ800);

MFD *mfd;

void setup() {
  delay(3000);
  Serial.begin(115200);
  Serial.println("Starting...");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 1);

  // Initialize our I2C bus
  Wire1.begin();
  // BNO055 needs up to 1ms to read 6 registers
  Wire1.setTimeout(5000);
  Wire1.setOpMode(I2C_OP_MODE_IMM);

  ILI9341Display *display = new ILI9341Display(display_mosi, display_miso, display_sck, display_cs, display_dc, display_backlight, display_rotation, Size(display_width, display_height));
  Encoder *encoder = new Encoder(encoder_a, encoder_b);
  pinMode(encoder_button, INPUT_PULLUP);
  Bounce *button = new Bounce(encoder_button, 10 /* ms */);

  mfd = new MFD(*display, *encoder, *button);

  EncoderTestPage *encPage = new EncoderTestPage();
  //mfd->addPage(encPage);

  WifiTestPage *wifiPage = new WifiTestPage();
  //mfd->addPage(wifiPage);

  ShuntMonitorPage *shuntPage = new ShuntMonitorPage();
  //mfd->addPage(shuntPage);

  //BarometerPage *barometerPage = new BarometerPage();
  //mfd->addPage(barometerPage);

  IMUPage *imuPage = new IMUPage();
  //mfd->addPage(imuPage);

  NMEA2000Page *nmea2000Page = new NMEA2000Page();
  //mfd->addPage(nmea2000Page);

  NMEAPage *nmeaPage = new NMEAPage();
  //mfd->addPage(nmeaPage);

  ADCPage *adcPage = new ADCPage();
  //mfd->addPage(adcPage);
  
  SdcardTestPage *sdcardPage = new SdcardTestPage();
  //mfd->addPage(sdcardPage);

  ClockPage *clockPage = new ClockPage();
  mfd->addPage(clockPage);

  strip.begin();
  strip.show();
}

void loop() {
  mfd->loop();
}
