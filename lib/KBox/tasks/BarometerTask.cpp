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

#include <Debug.h>
#include "BarometerTask.h"
#include "i2c_t3.h"
#include "../drivers/board.h"

#include "Adafruit_BMP280.h"

void BarometerTask::setup() {
  if (!bmp280.begin(bmp280_address)) {
    DEBUG("Error initializing BMP280");
    status = 1;
  }
  else {
    status = 0;
  }
}

void BarometerTask::fetchValues() {
  temperature = bmp280.readTemperature();
  pressure = bmp280.readPressure();

  DEBUG("Read temperature=%f and pressure=%f", temperature, pressure);
}

void BarometerTask::loop() {
  fetchValues();
}
