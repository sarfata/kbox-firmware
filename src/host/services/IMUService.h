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

#pragma once

#include <Adafruit_BNO055.h>
#include "common/os/Task.h"
#include "common/signalk/SKHub.h"
#include "host/config/IMUConfig.h"

class IMUService : public Task {
  private:
    // Re-save calibration every 30min.
    const unsigned int resaveCalibrationTimeMs = 1800000;

    // Used when converting to an int to store with calibration data
    // offset is an angle in radians (-3.14 -> 3.14) - scaled to -31459 ->
    // 31459 which fits in an int16.
    const int offsetScalingValueToInt = 10000;

    IMUConfig &_config;
    SKHub &_skHub;
    Adafruit_BNO055 bno055;
    uint8_t _sysCalib, _gyroCalib, _accelCalib, _magCalib;
    uint8_t _axisConfig, _signConfig;
    double _roll, _pitch, _heading;
    double _offsetRoll, _offsetPitch;
    elapsedMillis _timeSinceLastCalSave;

    bool restoreCalibration();
    bool saveCalibration();

  public:
    IMUService(IMUConfig &config, SKHub& skHub);
    void setup();
    void loop();

    /**
     * Use current roll and pitch as offset so that if KBox does not move,
     * future measurement will appear as 0 / 0.
     */
    void setRollPitchOffset();

    // sysCal '0' in NDOF mode means that the device has not yet found the 'north pole',
    // and orientation values will be off  The heading will jump to an absolute value
    // once the BNO finds magnetic north (the system calibrationData status jumps to 1 or higher).
    bool isMagCalibrated() {
      return _magCalib == 3 && _sysCalib > 0;
    };

    bool isRollAndPitchCalibrated() {
      return _accelCalib >= 2 && _gyroCalib >= 2;
    };

    void getLastValues(int &_sysCalibration, int &accelCalibration, double &pitch, double &roll, int &magCalibration, double &heading);

};
