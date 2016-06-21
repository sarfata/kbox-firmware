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

#include <Arduino.h>
#include <KBox.h>
#include <util/SlipStream.h>

KBox kbox;
bool flashMode = false;
SlipStream computerConnection(Serial, 4096);
SlipStream espConnection(Serial1, 4096);

#define ESP_CMD_FLASH_BEGIN 0x02
#define ESP_CMD_FLASH_END 0x04
#define ESP_CMD_SYNC 0x08

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(0);
  Serial1.setTimeout(0);

  kbox.setup();
  kbox.getNeopixels().setPixelColor(0, 0x00, 0x20, 0x00);
  kbox.getNeopixels().show();
  esp_init();
  esp_reboot_in_program();

  DEBUG_INIT();
  DEBUG("Starting esp program");
  Serial3.setTimeout(0);
}

struct ESPFrameHeader {
  uint8_t zero;
  uint8_t op;
  uint16_t size;
  uint32_t checksum;
};

void debug_frame(const char *name, uint8_t *ptr, size_t len) {
  static const int maxBytesDumped = 32;

  if (len < sizeof(ESPFrameHeader)) {
    DEBUG("%s: Packet is too small with %i bytes", name, len);
    return;
  }
  struct ESPFrameHeader *header = (struct ESPFrameHeader*)ptr;

  if (len != header->size + sizeof(struct ESPFrameHeader)) {
    DEBUG("%s: Packet Len=%i - zero=%x op=%x size=%i checksum=%i - INVALID DATA LENGTH len should be: %i", name, len, header->zero, header->op, header->size, header->checksum, header->size + sizeof(struct ESPFrameHeader));
  }
}

void loop() {
  digitalWrite(led_pin, 1);
  uint8_t buffer[4096];

  // On most ESP, the RTS line is used to reset the module
  // DTR is used to boot in flash - or - run program
  if (Serial.rts() && !flashMode) {
    DEBUG("rts asserted - going into flash mode");
    kbox.getNeopixels().setPixelColor(0, 0x20, 0x00, 0x00);
    kbox.getNeopixels().show();
    esp_reboot_in_flasher();
    flashMode = true;
  }

  if (computerConnection.available()) {
    digitalWrite(led_pin, 0);
    size_t len = computerConnection.readFrame(buffer, sizeof(buffer));
    debug_frame("Computer", buffer, len);

    if (len > sizeof(struct ESPFrameHeader)) {
      struct ESPFrameHeader *header = (struct ESPFrameHeader*) buffer;

      if (header->op == ESP_CMD_SYNC) {
        DEBUG("Sync");
        kbox.getNeopixels().setPixelColor(0, 0x00, 0xa5, 0xa5);
        kbox.getNeopixels().show();
      }
      if (header->op == ESP_CMD_FLASH_BEGIN) {
        DEBUG("Flash begin");
        kbox.getNeopixels().setPixelColor(0, 0xff, 0xa5, 0x00);
        kbox.getNeopixels().show();
      }
      if (header->op == ESP_CMD_FLASH_END) {
        DEBUG("Flash end");
        kbox.getNeopixels().setPixelColor(0, 0x00, 0x20, 0x00);
        kbox.getNeopixels().show();
        flashMode = false;
      }
    }
    espConnection.writeFrame(buffer, len);
  }

  if (espConnection.available()) {
    digitalWrite(led_pin, 0);
    size_t len = espConnection.readFrame(buffer, sizeof(buffer));
    debug_frame("ESP", buffer, len);
    computerConnection.writeFrame(buffer, len);
  }
}
