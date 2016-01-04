#include <stdio.h>
#include "EncoderTestPage.h"

bool EncoderTestPage::processEvent(const EncoderEvent &e) {
  /*
  counter += e.rotation;
  Serial.print("Rotating by ");
  Serial.println(e.rotation);
  needsPainting = true;
  */
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

uint32_t FreeRam(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is the free, available ram.
    return stackTop - heapTop;
}

bool EncoderTestPage::processEvent(const TickEvent &e) {
  timer = e.getMillis();

  DEBUG("tick free mem = %i", FreeRam());
  needsPainting = true;
  return true;
}

void EncoderTestPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  char text[20];

  //gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
  gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  Encoder, Button, Tick");


  gc.fillRectangle(Point(42, 80), Size(320 -42, 60), ColorBlue);
  snprintf(text, sizeof(text), "Encoder pos: %i", counter);
  gc.drawText(Point(42, 80), FontDefault, ColorWhite, text);

  snprintf(text, sizeof(text), "Clicks: %i", clicked);
  gc.drawText(Point(42, 100), FontDefault, ColorWhite, text);

  snprintf(text, sizeof(text), "Ticks: %lu", timer);
  gc.drawText(Point(42, 120), FontDefault, ColorWhite, text);

  needsPainting = false;
}
