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

class ShuntMonitorPage : public Page {
  private:
    static const int shuntVoltageRegisterAddress = 0x01;
    static const int busVoltageRegisterAddress = 0x02;
    int shuntVoltage;
    int busVoltage;

    static const int StatusOk = 0;
    static const int StatusNotReady = -1;
    int status = StatusNotReady;

    bool needsPainting;

    bool readRegister(uint8_t address, uint16_t *value);
    void fetchValues();

  public:
    ShuntMonitorPage();

    void willAppear();
    bool processEvent(const ButtonEvent &e);
    void paint(GC &gc);
};

