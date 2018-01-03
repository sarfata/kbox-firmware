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
#include "host/util/PersistentStorage.h"

void IMUService::setup() {
  _timeSinceLastSetOffset = 0;
  _offsetRoll  = 0.0;
  _offsetPitch = 0.0;
  // Default mounting position: VerticalStbHull
  uint8_t axisConfig = 0b00001001;
  uint8_t signConfig = 0b00000000;

  if (_config.mounting == VerticalTopToBow) {
    axisConfig = 0b00001001;
    signConfig = 0b00000000;
  }
  if (_config.mounting == HorizontalLeftSideToBow) {
    axisConfig = 0b00100100;
    signConfig = 0b00000000;
  }

  DEBUG("Initing BNO055");
  if (!bno055.begin(bno055.OPERATION_MODE_NDOF, axisConfig, signConfig)) {
    DEBUG("Error initializing BNO055");
  }
  else {
    // read calibration and offset values for heel & pitch from flash
    readCalibrationOffsets();
    readHeelPitchOffset();
  }
}

void IMUService::loop() {
  bno055.getCalibration(&_sysCalib, &_gyroCalib, &_accelCalib, &_magCalib);
  //DEBUG("Calib Sys: %i Accel: %i Gyro: %i Mag: %i", _sysCalib, _accelCalib, _gyroCalib, _magCalib);

  eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);
  //DEBUG("Sensor Raw-datas: eulerAngles.z=%.3f eulerAngles.y=%.3f  eulerAngles.x=%.3f", eulerAngles.z(), eulerAngles.y(), eulerAngles.x());

  SKUpdateStatic<2> update;
  // Note: We could calculate yaw as the difference between the Magnetic
  // Heading and the Course Over Ground Magnetic.

  if (_config.mounting == VerticalStbHull) {
    _roll = SKDegToRad(eulerAngles.z());
    _pitch = SKDegToRad(eulerAngles.y());
    _heading = SKDegToRad(fmod(eulerAngles.x() + 270, 360));
  }
  if (_config.mounting == VerticalTopToBow) {
    //TODO: check position for CaptainRon47
    _roll = SKDegToRad(eulerAngles.y());
    _pitch = SKDegToRad(eulerAngles.z());
    _heading = SKDegToRad(fmod(eulerAngles.x() + 180, 360));
  }
  if (_config.mounting == HorizontalLeftSideToBow) {
    _roll = SKDegToRad(eulerAngles.y());
    _pitch = SKDegToRad(eulerAngles.z()) * (-1);
    _heading = SKDegToRad(eulerAngles.x());
  }

  // update NavigationHeadingMagnetic if quality (= calibration value) is ok
  if (isMagCalibrated()) {
    update.setNavigationHeadingMagnetic(_heading);
  }
  // update NavigationAttitude if quality (= calibration value) is ok
  if (isHeelAndPitchCalibrated()) {
    update.setNavigationAttitude(SKTypeAttitude(/* roll */ _roll + _offsetRoll, /* pitch */ _pitch + _offsetPitch, /* yaw */ SKDoubleNAN));
    //DEBUG("Heel = %.3f | Pitch = %.3f | Heading = %.3f", SKRadToDeg( _roll + _offsetRoll), SKRadToDeg( _pitch + _offsetPitch), SKRadToDeg(_heading));
  }
  _skHub.publish(update);

  // Save calibration values to EEPROM every 30 Minutes, if BNO055 is fully calibrated
  if (bno055.isFullyCalibrated() && _timeSinceLastCalSave > 1800000) {
    if (saveCalibrationOffsets())
      _timeSinceLastCalSave = 0;
  }
}

// get the actual values of calibration, heel & pitch (including offset) and heading for display
// all angles in radians
void IMUService::getLastValues(int &sysCalibration, int &accelCalibration, double &pitch, double &roll, int &magCalibration, double &heading){
  sysCalibration = _sysCalib;
  accelCalibration = _accelCalib;
  pitch = _pitch + _offsetPitch;
  roll = _roll + _offsetRoll;
  magCalibration = _magCalib;
  heading = _heading;
}

//  Calc offset for making Heel=0 and Pitch=0
//  (e.g. called with long button press in IMUMonitorPage)
//  If an setOffset will be done a second time within 5 Seconds,
//  the values will be stored into EEPROM and loaded at start of KBox
void IMUService::setHeelPitchOffset() {
  bool changed = false;
  // FIXIT: Why is it called at startup of KBox? event longClick coming?
  if ( millis() > 10000 ) {
    if (abs(_roll) < M_PI / 2 ) {
      _offsetRoll = _roll * (-1);
      changed = true;
    }
    if (abs(_pitch) < M_PI / 2 ) {
      _offsetPitch = _pitch * (-1);
      changed = true;
    }
    if ( changed ) {
      INFO("Offset changed: Heel -> %.3f | Pitch -> %.3f", SKRadToDeg(_offsetRoll), SKRadToDeg(_offsetPitch));
      if (_timeSinceLastSetOffset < 5000) {
        saveHeelPitchOffset();
      }
    }
    _timeSinceLastSetOffset = 0;
  }
}

bool IMUService::saveHeelPitchOffset() {
  struct PersistentStorage::IMUHeelPitchOffsets o;
  o.offsetHeel = (int) _offsetRoll * 1000;
  o.offsetPitch = (int) _offsetPitch * 1000;

  if (PersistentStorage::writeImuHeelPitchOffsets(o)) {
    DEBUG("Storing offset data to EEPROM...");
    return true;
  }
  ERROR("Error saving IMU Offsets for heel and pitch to flash.");
  return false;
}

bool IMUService::readHeelPitchOffset() {
  struct PersistentStorage::IMUHeelPitchOffsets o;

  if (PersistentStorage::writeImuHeelPitchOffsets(o)) {
    DEBUG("Recalling offset data from EEPROM...");
    _offsetRoll = o.offsetHeel / 1000.0;
    _offsetPitch = o.offsetPitch / 1000.0;
    return true;
  }
  ERROR("Error reading IMU Offsets for heel and pitch from flash.");
  return false;
}

bool IMUService::saveCalibrationOffsets() {
  adafruit_bno055_offsets_t c;
  bno055.getSensorOffsets(c);

  struct PersistentStorage::BNO055CalOffsets o;
  o.accel_offset_x = c.accel_offset_x;
  o.accel_offset_y = c.accel_offset_y;
  o.accel_offset_z = c.accel_offset_z;
  o.gyro_offset_x = c.gyro_offset_x;
  o.gyro_offset_y = c.gyro_offset_y;
  o.gyro_offset_z = c.gyro_offset_z;
  o.mag_offset_x = c.mag_offset_x;
  o.mag_offset_y = c.mag_offset_y;
  o.mag_offset_z = c.mag_offset_z;
  o.accel_radius = c.accel_radius;
  o.mag_radius = c.mag_radius;

  if (PersistentStorage::writeBNO055CalOffsets(o)) {
    DEBUG("BNO055 Calibration Offsets written to flash.");
    return true;
  }
  ERROR("Error saving BNO055 Calibration Offsets to flash.");
  return false;
}

bool IMUService::readCalibrationOffsets() {
  adafruit_bno055_offsets_t c;
  struct PersistentStorage::BNO055CalOffsets o;

  if (PersistentStorage::readBNO055CalOffsets(o)) {
    c.accel_offset_x = o.accel_offset_x;
    c.accel_offset_y = o.accel_offset_y;
    c.accel_offset_z = o.accel_offset_z;
    c.gyro_offset_x = o.gyro_offset_x;
    c.gyro_offset_y = o.gyro_offset_y;
    c.gyro_offset_z = o.gyro_offset_z;
    c.mag_offset_x = o.mag_offset_x;
    c.mag_offset_y = o.mag_offset_y;
    c.mag_offset_z = o.mag_offset_z;
    c.accel_radius = o.accel_radius;
    c.mag_radius = o.mag_radius;

    bno055.setSensorOffsets(c);
    DEBUG("BNO055 Calibration Offsets recalled from flash.");
    return true;
  }
  ERROR("Error recalling BNO055 Calibration Offsets from EEPROM");
  return false;
}
