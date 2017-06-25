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
#include "IMUTask.h"
#include "KBoxDebug.h"

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

  eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);

  double eheading, epitch, eroll;
  switch(state){
    case PORT:
      eheading = eulerAngles.x() - 90;
      epitch = eulerAngles.y() * -1;
      eroll = eulerAngles.z();
      break;
    case STBD:
      eheading = eulerAngles.x() + 90;
      epitch = eulerAngles.y();
      eroll = eulerAngles.z() * -1;
      break;
    case BOW:
      eheading = eulerAngles.x() - 180;
      epitch = eulerAngles.z();
      eroll = eulerAngles.y();
      break;
    case STERN:
      eheading = eulerAngles.x();
      epitch = eulerAngles.z() * -1;
      eroll = eulerAngles.y() * -1;
      break;
  }
  IMUMessage m(sysCalib, DegToRad(eheading), /* yaw?*/ 0, DegToRad(epitch), DegToRad(eroll));
  sendMessage(m);
}
