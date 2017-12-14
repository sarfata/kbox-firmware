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

class IMUService : public Task {
  private:
    SKHub &_skHub;
    Adafruit_BNO055 bno055;
    uint8_t _sysCalib, _gyroCalib, _accelCalib, _magCalib;
    double _roll, _pitch, _heading;
    double _offsetRoll, _offsetPitch;
    imu::Vector<3> eulerAngles;

    // TODO: change to config setting
    uint8_t _cfHdgMinCal, _cfHeelPitchMinCal;

  public:
    IMUService(SKHub& skHub) : Task("IMU"), _skHub(skHub) {};
    void setup();
    void loop();

    void getLastValues(int &accelCalibration, double &pitch, double &roll, int &magCalibration, double &heading);
    void resetIMU();
    bool recallCalibration();
    bool saveCalibration();
    void setOffset();
};
