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

#include <Time.h>
#include "ClockPage.h"

time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

ClockPage::ClockPage() {
  setSyncProvider(getTeensy3Time);

  delay(100);
  if (timeStatus() != timeSet) {
    DEBUG("Unable to sync with RTC");
  }
  else {
    DEBUG("Sync'd with RTC");
  }
}

void ClockPage::willAppear() {
  needsPainting = true;

}

bool ClockPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    time_t t = 1453057665L;
    setTime(t);
    Teensy3Clock.set(t);
  }
  return true;
}

bool ClockPage::processEvent(const TickEvent &e) {
  static time_ms_t lastUpdate = 0;

  if (e.getMillis() + 500 > lastUpdate) {
    needsPainting = true;
    lastUpdate = e.getMillis();
  }
  return true;
}

void ClockPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  char text[20];

  gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
  gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  Clock");

  DEBUG("Printing time...");
  time_t t = now();
  snprintf(text, sizeof(text), "%02i:%02i:%02i", hour(t), minute(t), second(t));
  gc.drawText(Point(20, 80), FontDefault, ColorWhite, text);

  needsPainting = false;
}

