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
