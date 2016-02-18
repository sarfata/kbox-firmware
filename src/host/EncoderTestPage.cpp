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

#include <stdio.h>
#include "EncoderTestPage.h"

bool EncoderTestPage::processEvent(const EncoderEvent &e) {
  counter += e.rotation;
  Serial.print("Rotating by ");
  Serial.println(e.rotation);
  needsPainting = true;
  return true;
}

bool EncoderTestPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    clicked++  ;
    longClickTrigger = millis() + 1000;
  }
  if (e.clickType == ButtonEventTypeReleased) {
    longClickTrigger = 0;
  }
  Serial.println("Button pressed/released");
  needsPainting = true;
  return true;
}

bool EncoderTestPage::processEvent(const TickEvent &e) {
  if (longClickTrigger && millis() > longClickTrigger) {
    DEBUG("long click");
    longClickTrigger = 0;
    return false;
  }
  timer = e.getMillis();
  needsPainting = true;
  return true;
}

void EncoderTestPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  char text[20];

  //gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
  gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  Encoder, Button, Tick");


//  gc.fillRectangle(Point(42, 80), Size(320 -42, 60), ColorBlue);
//
  snprintf(text, sizeof(text), "Encoder pos: %3i  ", counter);
  gc.drawText(Point(42, 80), FontDefault, ColorWhite, text);

  snprintf(text, sizeof(text), "Clicks: %i  ", clicked);
  gc.drawText(Point(42, 120), FontDefault, ColorWhite, text);

  snprintf(text, sizeof(text), "Ticks: %lu", timer);
  gc.drawText(Point(42, 160), FontDefault, ColorWhite, text);

  needsPainting = false;
}
