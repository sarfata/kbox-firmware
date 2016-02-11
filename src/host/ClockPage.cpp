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
  static time_t lastUpdate = 0;

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

