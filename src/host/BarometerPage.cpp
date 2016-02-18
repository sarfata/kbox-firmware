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

#include "BarometerPage.h"
#include "i2c_t3.h"
#include "board.h"

#include "Adafruit_BMP280.h"

BarometerPage::BarometerPage() {
  if (!bmp280.begin(bmp280_address)) {
    DEBUG("Error initializing BMP280");
    status = 1;
  }
  else {
    status = 0;
  }
}

void BarometerPage::willAppear() {
  fetchValues();
  needsPainting = true;
}

void BarometerPage::fetchValues() {
  uint8_t reg;

  temperature = bmp280.readTemperature();
  pressure = bmp280.readPressure();

  DEBUG("Read temperature=%f and pressure=%f", temperature, pressure);
}

bool BarometerPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    fetchValues();
    needsPainting = true;
  }
  return true;
}

void BarometerPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
  gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  Barometer Test Page");

  char text[30];
  if (status == 0) {
    snprintf(text, sizeof(text), "Temperature: %f", temperature);
    gc.drawText(Point(0, 30), FontDefault, ColorWhite, text);
    snprintf(text, sizeof(text), "Pressure: %f", pressure);
    gc.drawText(Point(0, 60), FontDefault, ColorWhite, text);
  } else {
    snprintf(text, sizeof(text), "Error: %i", status);
    gc.drawText(Point(0, 30), FontDefault, ColorRed, text);
  }

  needsPainting = false;
}
