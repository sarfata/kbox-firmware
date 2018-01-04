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

IMUService::IMUService(IMUConfig &config, SKHub &skHub) :
  Task("IMU"),  _config(config), _skHub(skHub),
  _timeSinceLastCalSave(resaveCalibrationTimeMs) {

}

void IMUService::setup() {
  _offsetRoll  = 0.0;
  _offsetPitch = 0.0;

  // Remap is 00zzyyxx
  // Where each can be 00:X, 01: Y, 10:Z
  // Sign is the 3 lsb: 00000xyz
  switch (_config.mounting) {
    case HorizontalLeftSideToBow:
      _axisConfig = 0b00100100;
      _signConfig = 0b00000000;
    break;
    case VerticalTopToBow:
    case VerticalStbHull:
    case VerticalPortHull:
      _axisConfig = 0b00001001;
      _signConfig = 0b00000000;
  }

  DEBUG("Initing BNO055");
  if (!bno055.begin(bno055.OPERATION_MODE_NDOF, _axisConfig, _signConfig)) {
    DEBUG("Error initializing BNO055");
  }
  else {
    // read calibrationData and offset values for heel & pitch from flash
    restoreCalibration();
  }
}

void IMUService::loop() {
  bno055.getCalibration(&_sysCalib, &_gyroCalib, &_accelCalib, &_magCalib);
  imu::Vector<3> eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);

  SKUpdateStatic<2> update;

  // In the SignalK Specification
  //  roll:   Vessel roll, +ve is list to starboard
  //  pitch:  Pitch, +ve is bow up
  switch (_config.mounting) {
    case VerticalPortHull:
      _roll = SKDegToRad(eulerAngles.z());
      _pitch = SKDegToRad(eulerAngles.y());
      _heading = SKDegToRad(fmod(eulerAngles.x() + 270, 360));
    break;
    case VerticalStbHull:
      _roll = SKDegToRad(eulerAngles.z())*(-1);
      _pitch = SKDegToRad(eulerAngles.y())*(-1);
      _heading = SKDegToRad(eulerAngles.x());
    break;
    case VerticalTopToBow:
      _roll = SKDegToRad(eulerAngles.y());
      _pitch = SKDegToRad(eulerAngles.z());
      _heading = SKDegToRad(fmod(eulerAngles.x() + 180, 360));
    break;
    case HorizontalLeftSideToBow:
      _roll = SKDegToRad(eulerAngles.y());
      _pitch = SKDegToRad(eulerAngles.z()) * (-1);
      _heading = SKDegToRad(eulerAngles.x());
    break;
  }

  if (isMagCalibrated()) {
    update.setNavigationHeadingMagnetic(_heading);
  }

  if (isRollAndPitchCalibrated()) {
    // Yaw is always NAN. We cannot really measure it with our instruments.
    update.setNavigationAttitude(SKTypeAttitude( _roll + _offsetRoll,
                                                 _pitch + _offsetPitch,
                                                 SKDoubleNAN));
  }
  _skHub.publish(update);

  // Save calibrationData values to EEPROM every 30 Minutes, if BNO055 is fully calibrated
  if (bno055.isFullyCalibrated() &&
      _timeSinceLastCalSave > resaveCalibrationTimeMs) {
    saveCalibration();
    _timeSinceLastCalSave = 0;
  }
}

// get the actual values of calibrationData, heel & pitch (including offset) and heading for display
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
void IMUService::setRollPitchOffset() {
  bool changed = false;
  if (abs(_roll) < M_PI / 2 ) {
    _offsetRoll = _roll * (-1);
    changed = true;
  }
  if (abs(_pitch) < M_PI / 2 ) {
    _offsetPitch = _pitch * (-1);
    changed = true;
  }

  if ( changed ) {
    INFO("Offset changed: Heel -> %.3f | Pitch -> %.3f",
         SKRadToDeg(_offsetRoll), SKRadToDeg(_offsetPitch));
    DEBUG("Offset changed: Heel -> %.3f | Pitch -> %.3f", _offsetRoll,
          _offsetPitch);
    saveCalibration();
    _timeSinceLastCalSave = 0;
  }
}

bool IMUService::saveCalibration() {
  struct PersistentStorage::IMUCalibration calibration;

  // This will verify at compile time that there is enough space in the
  // persistent storage structure to save all the calibration registers.
  static_assert(sizeof(calibration.calibrationData) ==
                NUM_BNO055_OFFSET_REGISTERS,
    "Not enough space in PersistentStorage::IMUCalibration to store BNO055 "
      "calibration data.");

  calibration.mountingPosition = static_cast<uint8_t>(_config.mounting);
  if (! bno055.getSensorOffsets(calibration.calibrationData)) {
    ERROR("Cannot save calibration while not fully calibrated.");
    return false;
  }
  calibration.offsetPitch =
    static_cast<int>(_offsetPitch * offsetScalingValueToInt);
  calibration.offsetRoll =
    static_cast<int>(_offsetRoll * offsetScalingValueToInt);

  if (PersistentStorage::writeIMUCalibration(calibration)) {
    DEBUG("IMU Calibration written to flash.");
    return true;
  } else {
    ERROR("Error saving IMU Calibration to flash.");
    return false;
  }
}

bool IMUService::restoreCalibration() {
  struct PersistentStorage::IMUCalibration calibration;

  if (PersistentStorage::readIMUCalibration(calibration)) {
    if (calibration.mountingPosition != _config.mounting) {
      DEBUG("Mounting position has changed. Discarding previous calibration.");
      return false;
    }
    bno055.setSensorOffsets(calibration.calibrationData);
    _offsetPitch = calibration.offsetPitch / offsetScalingValueToInt;
    _offsetRoll = calibration.offsetRoll / offsetScalingValueToInt;
    DEBUG("IMU Calibration recalled from flash.");
    return true;
  } else {
    ERROR("Error recalling IMU Calibration from flash");
    return false;
  }
}

