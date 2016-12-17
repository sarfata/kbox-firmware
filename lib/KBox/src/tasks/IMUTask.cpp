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

#include <KBoxLogging.h>
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
  //uint8_t system_status, self_test_status, system_error;
  //bno055.getSystemStatus(&system_status, &self_test_status, &system_error);
  //DEBUG("BNO055 System Status: %x Self-Test Status: %x System Error: %x", system_status, self_test_status, system_error);

  bno055.getCalibration(&sysCalib, &gyroCalib, &accelCalib, &magCalib);
  //DEBUG("BNO055 Calibration - Sys:%i Gyro:%i Accel:%i Mag:%i", sysCalib, gyroCalib, accelCalib, magCalib);


  {
    eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);
    //DEBUG("Vector Euler x=%f y=%f z=%f", eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
    //DEBUG("Course: %.0f MAG  Pitch: %.1f  Heel: %.1f", eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
    IMUMessage m(sysCalib, eulerAngles.x(), /* yaw?*/ 0, eulerAngles.y(), eulerAngles.z());
    sendMessage(m);
  }
  
  //DEBUG("BNO055 temperature is %i", bno055.getTemp());

  //{
    //imu::Vector<3> vector = bno055.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
    //DEBUG("Vector Magnetometer x=%f y=%f z=%f", vector.x(), vector.y(), vector.z());
  //}
  //{
    //imu::Vector<3> vector = bno055.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    //DEBUG("Vector Gyro x=%f y=%f z=%f", vector.x(), vector.y(), vector.z());
  //}
  //{
    //imu::Vector<3> vector = bno055.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    //DEBUG("Vector Accel x=%f y=%f z=%f", vector.x(), vector.y(), vector.z());
  //}
  //{
    //imu::Vector<3> vector = bno055.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
    //DEBUG("Vector Gravity x=%f y=%f z=%f", vector.x(), vector.y(), vector.z());
  //}
}
