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

#include <ILI9341_t3.h>
#include <font_Nunito.h>
#include "Display.h"
#include "pin.h"

class ILI9341Display : public Display {
  private:
    ILI9341_t3 *display;
    Size size;
    pin_t backlightPin;

  public:
    ILI9341Display();

    /* Display interface */
    const Size& getSize() const {
      return size;
    }

    void setBacklight(BacklightIntensity intensity);

    /* GC interface */
    void drawText(Point a, Font font, Color color, const char *text);
    void drawLine(Point a, Point b, Color color);
    void drawRectangle(Point orig, Size size, Color color);
    void fillRectangle(Point orig, Size size, Color color);
};
