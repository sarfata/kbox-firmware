#include "WifiTestPage.h"
#include "hardware/board.h"

WifiTestPage::WifiTestPage() {
  Serial1.begin(115200);

  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_program, 0);
  digitalWrite(wifi_rst, 0);

  pinMode(wifi_enable, OUTPUT);
  pinMode(wifi_program, OUTPUT);
  pinMode(wifi_rst, OUTPUT);

  // To boot from Flash, we need
  // GPIO0 (wifi_program) HIGH and GPIO2 HIGH (hard-wired)
  // GPIO15 (wifi_cs) LOW (hard-wired)
  // Then we need RST LOW and we set ENABLE HIGH
  digitalWrite(wifi_program, HIGH);
  //digitalWrite(wifi_enable, HIGH);
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
