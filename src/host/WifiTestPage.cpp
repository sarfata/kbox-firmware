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
#include "WifiTestPage.h"
#include "board.h"

WifiTestPage::WifiTestPage() {
  Serial1.begin(115200);

  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_program, 0);
  digitalWrite(wifi_rst, 0);

  pinMode(wifi_enable, OUTPUT);
  pinMode(wifi_program, OUTPUT);
#ifndef BOARD_v1_revA
  pinMode(wifi_cs, OUTPUT);
  digitalWrite(wifi_cs, 0);
#endif
  pinMode(wifi_rst, OUTPUT);

  // To boot from Flash, we need
  // GPIO0 (wifi_program) HIGH and GPIO2 HIGH (pulled up via a resistor)
  // GPIO15 (wifi_cs) LOW
  // Then we need RST LOW and we set ENABLE HIGH
  digitalWrite(wifi_program, HIGH);

  DEBUG("WifiTestPage Constructor");
}

void WifiTestPage::willAppear() {
  DEBUG("Wifi page will appear");
  needsPainting = true;
  needsFullPainting = true;

  // release reset pin and boot!
  digitalWrite(wifi_enable, 1);
  digitalWrite(wifi_rst, 1);
}

bool WifiTestPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    DEBUG("Getting status...");
    Serial1.println("AT+PING=\"www.google.com\"");
    return true;
  }
  return true;
}

bool WifiTestPage::processEvent(const TickEvent &e) {
  int available = Serial1.available();
  if (available > 0) {
    // Limit reading to space left in buffer.
    if (available > bufferSize - bufferIndex) {
      available = bufferSize - bufferIndex - 1;
    }
    int read = Serial1.readBytes(buffer + bufferIndex, available);
    bufferIndex = bufferIndex + read;
    buffer[bufferIndex] = 0;
    needsPainting = true;
    DEBUG("Read %i bytes >%s<", read, buffer + bufferIndex - read);

    if (strstr(buffer, "ready") != 0) {
      bufferIndex = 0;
      DEBUG("Sending AT+GMR");
      Serial1.println("AT+GMR");
    }
    if (strstr(buffer, "version") != 0) {
      bufferIndex = 0;
      DEBUG("Sending AT+CWMODE=1");
      Serial1.println("AT+CWMODE=1");
    }
    if (strstr(buffer, "AT+CWMODE") != 0) {
      bufferIndex = 0;
      DEBUG("Sending AT+CWLAP");
      Serial1.println("AT+CWLAP");
    }

  }
  return true;
}

void WifiTestPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  if (1 || needsFullPainting) {
    gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
    gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  Wifi Test Page");
    needsFullPainting = false;
  }

  gc.drawText(Point(0, 30), FontDefault, ColorWhite, buffer);
  needsPainting = false;
}
