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
#include <EEPROM.h>

void IMUService::setup() {
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
    if (recallCalibration())
      DEBUG("Success!");
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
}

// get the actual values of calibration, heel & pitch (including offset) and heading for display
// all angles in radians
void IMUService::getLastValues(int &accelCalibration, double &pitch, double &roll, int &magCalibration, double &heading){
  accelCalibration = _accelCalib;
  pitch = _pitch + _offsetPitch;
  roll = _roll + _offsetRoll;
  magCalibration = _magCalib;
  heading = _heading;
}

//  Calc offset for making Heel=0 and Pitch=0
//  (e.g. called with long button press in IMUMonitorPage)
//  TODO: If an setOffset will be done a second time within 5 Seconds,
//  the values will be stored into EEPROM and loaded at start of KBox
void IMUService::setOffset() {
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
    }
  }
}

bool IMUService::saveCalibration() {
  int _eeAddress = 0;
  long bnoID;
  sensor_t sensor;
  adafruit_bno055_offsets_t newCalib;
  bno055.getSensorOffsets(newCalib);

  DEBUG("Storing calibration data to EEPROM...");
  bno055.getSensor(&sensor);
  bnoID = sensor.sensor_id;

  EEPROM.put(_eeAddress, bnoID);

  _eeAddress += sizeof(long);
  EEPROM.put(_eeAddress, newCalib);
  DEBUG("Data stored to EEPROM.");
  delay(500);
  return true;
}

bool IMUService::recallCalibration() {

  int _eeAddress = 0;
  long bnoID;

  EEPROM.get(_eeAddress, bnoID);
  adafruit_bno055_offsets_t calibrationData;
  sensor_t sensor;

  /*
  *  Look for the sensor's unique ID at the beginning oF EEPROM.
  *  This isn't foolproof, but it's better than nothing.
  */
  bno055.getSensor(&sensor);
  if (bnoID != sensor.sensor_id) {
      DEBUG("No Calibration Data for this sensor exists in EEPROM");
  }
  else {
      DEBUG("Found Calibration for this sensor in EEPROM.");
      _eeAddress += sizeof(long);
      EEPROM.get(_eeAddress, calibrationData);

      DEBUG("Restoring Calibration data to the BNO055...");
      bno055.setSensorOffsets(calibrationData);

      DEBUG("Calibration data loaded into BNO055");
      delay(1000);
      return true;
  }
  return false;
}
