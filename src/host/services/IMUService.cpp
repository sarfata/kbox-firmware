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
#include "common/signalk/SKUpdateStatic.h"
#include "IMUService.h"

void IMUService::setup() {
  DEBUG("Initing BNO055");
  if (!bno055.begin()) {
    DEBUG("Error initializing BNO055");
  }
  else {
    DEBUG("Success!");
  }
}

void IMUService::loop() {
  bno055.getCalibration(&sysCalib, &gyroCalib, &accelCalib, &magCalib);

  eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);

  SKUpdateStatic<2> update;
  // Note: We could calculate yaw as the difference between the Magnetic
  // Heading and the Course Over Ground Magnetic.
  update.setNavigationAttitude(SKTypeAttitude(/* roll */ eulerAngles.z(), /* pitch */ eulerAngles.y(), /* yaw */ 0));
  update.setNavigationHeadingMagnetic(eulerAngles.x());
  _skHub.publish(update);
}
