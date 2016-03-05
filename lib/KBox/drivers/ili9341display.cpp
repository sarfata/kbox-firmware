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

#include "ili9341display.h"
#include "board.h"

ILI9341Display::ILI9341Display() : size(Size(display_width, display_height)), backlightPin(display_backlight)
{
  display = new ILI9341_t3(display_cs, display_dc, 255 /* rst unused */, display_mosi, display_sck, display_miso);

  display->begin();
  display->fillScreen(ILI9341_BLUE);
  display->setRotation(display_rotation);

  setBacklight(BacklightIntensityMax);
}

void ILI9341Display::setBacklight(BacklightIntensity intensity) {
  if (backlightPin) {
    if (intensity > 0) {
      analogWrite(backlightPin, intensity);
    }
    else {
      analogWrite(backlightPin, 0);
    }
  }
}

void ILI9341Display::drawText(Point a, Font font, Color color, const char *text) {
  display->setCursor(a.x(), a.y());
  display->setTextColor(color, ColorBlue);
  display->setFont(Nunito_18);
  display->println(text);
}

void ILI9341Display::drawLine(Point a, Point b, Color color) {
  display->drawLine(a.x(), a.y(), b.x(), b.y(), color);
}

void ILI9341Display::drawRectangle(Point orig, Size size, Color color) {
  display->drawRect(orig.x(), orig.y(), size.width(), size.height(), color);
}

void ILI9341Display::fillRectangle(Point orig, Size size, Color color) {
  display->fillRect(orig.x(), orig.y(), size.width(), size.height(), color);
}

