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

#pragma once

#include "TaskManager.h"
#include "KMessage.h"
#include "Debug.h"

class WiFiTask : public Task, public KReceiver {
  public:
    WiFiTask() {};

    void setup();
    void loop();

    void processMessage(const KMessage &m) {
      //DEBUG("Wifi message: %s", m.toString().c_str());
    };
};

