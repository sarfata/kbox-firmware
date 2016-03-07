/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <Adafruit_BNO055.h>
#include "TaskManager.h"

class IMUTask : public Task {
  private:
    Adafruit_BNO055 bno055;
    uint8_t sysCalib, gyroCalib, accelCalib, magCalib;
    imu::Vector<3> eulerAngles;

  public:
    void setup();
    void loop();
};
