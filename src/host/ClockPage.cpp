/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
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

  time_t t = now();
  snprintf(text, sizeof(text), "%02i:%02i:%02i", hour(t), minute(t), second(t));
  gc.drawText(Point(20, 80), FontDefault, ColorWhite, text);

  needsPainting = false;
}

