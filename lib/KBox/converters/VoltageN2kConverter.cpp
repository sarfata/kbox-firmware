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

#include "VoltageN2kConverter.h"
#include <N2kMessages.h>

void VoltageN2kConverter::processMessage(const KMessage &m) {
  VoltageMeasurement *vm = (VoltageMeasurement*) &m;

  tN2kMsg msg;
  static int sid = 0;
  // iNavX does not seem to like NA as a current value
  SetN2kPGN127508(msg, vm->getIndex(), vm->getVoltage(), 0, N2kDoubleNA, sid++);
  sendMessage(NMEA2000Message(msg));
}
