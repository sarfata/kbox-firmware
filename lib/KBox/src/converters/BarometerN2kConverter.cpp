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

#include "BarometerN2kConverter.h"
#include <N2kMessages.h>

void BarometerN2kConverter::processMessage(const KMessage &m) {
  if (m.getMessageType() == KMessageType::BarometerMeasurement) {
    const BarometerMeasurement &bm = static_cast<const BarometerMeasurement&>(m);
    tN2kMsg n2km;
    static int sid = 0;
    SetN2kPGN130311(n2km, sid++, N2kts_MainCabinTemperature, CToKelvin(bm.getTemperature()),
        N2khs_InsideHumidity, 0.42, bm.getPressure());
    sendMessage(NMEA2000Message(n2km));
  }
}
