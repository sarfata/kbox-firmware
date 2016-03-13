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

// completely arbitrary value. "ought to be enough..."
#define MAX_NMEA_SENTENCE_LENGTH 200

class NMEAReaderTask : public Task {
  private:
    Stream& stream;
    int index;
    uint8_t buffer[MAX_NMEA_SENTENCE_LENGTH];

  public:
    NMEAReaderTask(Stream &s) : stream(s) {};

    void setup();
    void loop();
};

