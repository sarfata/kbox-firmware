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
#include "IMUTask.h"

void IMUTask::setup() {
  DEBUG("Initing BNO055");
  if (!bno055.begin()) {
    DEBUG("Error initializing BNO055");
  }
  else {
    DEBUG("Success!");
  }
}

void IMUTask::loop() {
  uint8_t reg;

  uint8_t system_status, self_test_status, system_error;
  bno055.getSystemStatus(&system_status, &self_test_status, &system_error);
  DEBUG("BNO055 System Status: %x Self-Test Status: %x System Error: %x", system_status, self_test_status, system_error);

  bno055.getCalibration(&sysCalib, &gyroCalib, &accelCalib, &magCalib);
  DEBUG("BNO055 Calibration - Sys:%i Gyro:%i Accel:%i Mag:%i", sysCalib, gyroCalib, accelCalib, magCalib);

  //DEBUG("BNO055 temperature is %i", bno055.getTemp());

  {
    eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);
    //DEBUG("Vector Euler x=%f y=%f z=%f", eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
    DEBUG("Course: %.0f MAG  Pitch: %.1f  Heel: %.1f", eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
  }
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
