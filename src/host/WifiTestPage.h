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

#include <MFD.h>

class WifiTestPage : public Page {
  private:
    bool needsPainting;
    bool needsFullPainting;
    static const int bufferSize = 1024;
    int bufferIndex = 0;
    char buffer[bufferSize];

  public:
    WifiTestPage();

    bool processEvent(const ButtonEvent &e);
    bool processEvent(const TickEvent &e);
    void willAppear();
    void paint(GC &gc);
};
