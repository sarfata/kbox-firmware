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
#include "common/signalk/SKUnits.h"
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

  //DEBUG("Calib Sys: %i Accel: %i Gyro: %i Mag: %i", sysCalib, accelCalib, gyroCalib, magCalib);
  //DEBUG("Attitude roll: %f pitch: %f  Mag heading: %f", eulerAngles.z(), eulerAngles.y(), eulerAngles.x());

  SKUpdateStatic<2> update;
  // Note: We could calculate yaw as the difference between the Magnetic
  // Heading and the Course Over Ground Magnetic.

  /* if orientation == MOUNTED_ON_PORT_HULL */
  double roll, pitch, heading;
  roll = eulerAngles.z();
  pitch = eulerAngles.y();
  heading = fmod(eulerAngles.x() + 270, 360);

  if (sysCalib == 3) {
    update.setNavigationAttitude(SKTypeAttitude(/* roll */ SKDegToRad(roll), /* pitch */ SKDegToRad(pitch), /* yaw */ SKDoubleNAN));
    update.setNavigationHeadingMagnetic(SKDegToRad(heading));
    _skHub.publish(update);
  }
}
