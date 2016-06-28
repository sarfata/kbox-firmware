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
#include "drivers/board.h"

void IMUTask::setup() {
  resetIMU();
}

void IMUTask::loop() {
  bno055.getCalibration(&sysCalib, &gyroCalib, &accelCalib, &magCalib);

  eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);
  IMUMessage m(sysCalib, gyroCalib, accelCalib, magCalib, eulerAngles.x(), eulerAngles.y(), eulerAngles.z());
  sendMessage(m);
}

void IMUTask::resetIMU() {
  /*
   * Note: we could try to reset the IMU via the reset line but it seems to
   * not wake up nicely from any type of reboot (software or hardware). There
   * is some mystery here that we will have to investigate.
   */

  if (!bno055.begin()) {
    DEBUG("Error initializing BNO055");
  }
  else {
    if (hasCalibData) {
      DEBUG("BNO055 started - Loading calibration data.");
      bno055.setSensorOffsets(calibData);
    }
    else {
      DEBUG("BNO055 started - No calibration data to load.");
    }
  }
}

bool IMUTask::saveCalibration() {

  if (bno055.getSensorOffsets(calibData)) {
    DEBUG("Saved calib data.");
    hasCalibData = true;
  }
  else {
    DEBUG("error getting calib data");
  }

  return false;
}
