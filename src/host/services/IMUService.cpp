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
#include "KBoxConfig.h"

// make it global to get the value for the IMU-Page
uint8_t IMUService::magCAL = 0;
double IMUService::IMU_HdgFiltered = 361;   // not valid

void IMUService::setup() {
  DEBUG("Initing BNO055");
  if (!bno055.begin()) {
    DEBUG("Error initializing BNO055");
  }
  else {
    // Different Mounting positions of KBox
    bno055.setMode(bno055.OPERATION_MODE_CONFIG);
    delay(50);
    switch ( cfKBoxOrientation ) {
      case MOUNTED_ON_PORT_HULL:
        bno055.write8(bno055.BNO055_AXIS_MAP_CONFIG_ADDR, 0b00001001); // P0-P7, Default is P1
        delay(10);
        bno055.write8(bno055.BNO055_AXIS_MAP_SIGN_ADDR, 0b00000000); // P0-P7, Default is P1
        delay(10);
      break;
      //case LAYING_READ_DIR_TO_BOW:
      //break;
      default:
        // Should be default setting of BNO055 Adafruit Library
        bno055.write8(bno055.BNO055_AXIS_MAP_CONFIG_ADDR, 0x21 );
        delay(10);
        bno055.write8(bno055.BNO055_AXIS_MAP_SIGN_ADDR, 0x04 );
        delay(10);
      break;
    }

    bno055.setMode( bno055.OPERATION_MODE_NDOF ); // OPERATION_MODE_NDOF_FMC_OFF
    delay(100);
    DEBUG("Success!");
  }
}

void IMUService::loop() {
  bno055.getCalibration(&sysCalib, &gyroCalib, &accelCalib, &magCalib);

  eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);

  // DEBUG("Calib Sys: %i Accel: %i Gyro: %i Mag: %i", sysCalib, accelCalib, gyroCalib, magCalib);
  // DEBUG("Attitude roll: %f pitch: %f  Mag heading: %f", eulerAngles.z(), eulerAngles.y(), eulerAngles.x());

  SKUpdateStatic<2> update;
  // Note: We could calculate yaw as the difference between the Magnetic
  // Heading and the Course Over Ground Magnetic.

  double roll, pitch, heading;
  roll = eulerAngles.z();
  pitch = eulerAngles.y();
  heading = eulerAngles.x();

  // additional calculation for KBox port mounting....
  switch ( cfKBoxOrientation ) {
    case MOUNTED_ON_PORT_HULL:
      heading = fmod(eulerAngles.x() + 270, 360);
    break;
    case LAYING_READ_DIR_TO_BOW:
      pitch = (-1) * eulerAngles.z();
      roll = eulerAngles.y();
    break;
    default:
    break;
  }

  if (magCalib >= cfIMU_MIN_CAL) {
    update.setNavigationHeadingMagnetic(SKDegToRad(heading));
  }
  
  if (gyroCalib >= cfIMU_MIN_CAL) {
    update.setNavigationAttitude(SKTypeAttitude(/* roll */ SKDegToRad(roll), /* pitch */ SKDegToRad(pitch), /* yaw */ 0));
  }
  
  // global variables for the IMU Page
  IMUService::magCAL = magCalib;
  // TODO: implement filtering or own visitor for display frequency
  IMUService::IMU_HdgFiltered = heading;  // Because it is for Display only we leave value in degrees

  _skHub.publish(update);
}
