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

#include "IMUPage.h"
#include <board.h>

IMUPage::IMUPage() {
  DEBUG("Initing BNO055");
  if (!bno055.begin()) {
    DEBUG("Error initializing BNO055");
    status = 1;
  }
  else {
    DEBUG("Success!");
    status = 0;
  }
}

void IMUPage::willAppear() {
  fetchValues();
  needsPainting = true;
}

void IMUPage::fetchValues() {
  uint8_t reg;

  uint8_t system_status, self_test_status, system_error;
  bno055.getSystemStatus(&system_status, &self_test_status, &system_error);
  DEBUG("BNO055 System Status: %x Self-Test Status: %x System Error: %x", system_status, self_test_status, system_error);

  bno055.getCalibration(&sysCalib, &gyroCalib, &accelCalib, &magCalib);
  DEBUG("BNO055 Calibration - Sys:%i Gyro:%i Accel:%i Mag:%i", sysCalib, gyroCalib, accelCalib, magCalib);

  DEBUG("BNO055 temperature is %i", bno055.getTemp());

  {
    eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);
    DEBUG("Vector Euler x=%f y=%f z=%f", eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
  }
  return;
  {
    imu::Vector<3> vector = bno055.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
    DEBUG("Vector Magnetometer x=%f y=%f z=%f", vector.x(), vector.y(), vector.z());
  }
  {
    imu::Vector<3> vector = bno055.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    DEBUG("Vector Gyro x=%f y=%f z=%f", vector.x(), vector.y(), vector.z());
  }
  {
    imu::Vector<3> vector = bno055.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    DEBUG("Vector Accel x=%f y=%f z=%f", vector.x(), vector.y(), vector.z());
  }
  {
    imu::Vector<3> vector = bno055.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
    DEBUG("Vector Gravity x=%f y=%f z=%f", vector.x(), vector.y(), vector.z());
  }
}

bool IMUPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    fetchValues();
    needsPainting = true;
  }
  return true;
}

bool IMUPage::processEvent(const TickEvent &e) {
  if (e.getMillis() > lastUpdate + UpdateInterval) {
    fetchValues();
    needsPainting = true;
  }
}

void IMUPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
  gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  IMU Test Page");

  char text[30];
  if (status == 0) {
    gc.drawText(Point(2, 40), FontDefault, ColorWhite, "Calibration Status (3=ok)");
    snprintf(text, sizeof(text), "Sys:%i Acc:%i Gyr:%i Mag:%i", sysCalib, accelCalib, gyroCalib, magCalib);
    gc.drawText(Point(2, 65), FontDefault, ColorWhite, text);

    gc.drawText(Point(2, 100), FontDefault, ColorWhite, "Euler Angles");
    snprintf(text, sizeof(text), "X: %.1f Y: %.1f Z:%.1f", eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
    gc.drawText(Point(2, 125), FontDefault, ColorWhite, text);
  } else {
    snprintf(text, sizeof(text), "Error: %f", status);
    gc.drawText(Point(0, 30), FontDefault, ColorRed, text);
  }

  needsPainting = false;
}
