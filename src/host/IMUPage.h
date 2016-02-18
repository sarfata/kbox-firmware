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

#include <MFD.h>
#include <Adafruit_BNO055.h>

class IMUPage : public Page {
  private:
    bool needsPainting;
    int status = -1;

    void fetchValues();

    uint8_t sysCalib, gyroCalib, accelCalib, magCalib;

    imu::Vector<3> eulerAngles;
    static const int UpdateInterval = 500;
    time_ms_t lastUpdate = 0;

    Adafruit_BNO055 bno055;

  public:
    IMUPage();

    void willAppear();
    bool processEvent(const ButtonEvent &e);
    bool processEvent(const TickEvent &e);
    void paint(GC &gc);
};

