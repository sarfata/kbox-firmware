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

#include <N2kMessages.h>
#include <NMEA2000_teensy.h>
#include <TaskManager.h>
#include "KMessage.h"

class NMEA2000Task : public Task, public KGenerator, public KReceiver {
  tNMEA2000_teensy NMEA2000;

  public:
    void setup();
    void loop();

    // Helper for the handler who is not a part of this class
    void publishN2kMessage(const tN2kMsg& msg);

    void processMessage(const KMessage&);
};
