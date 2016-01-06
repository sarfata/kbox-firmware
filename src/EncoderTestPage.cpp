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
    clicked++;
  }
  Serial.println("Button pressed/released");
  needsPainting = true;
  return true;
}

bool EncoderTestPage::processEvent(const TickEvent &e) {
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
