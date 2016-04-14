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

KBox kbox;
bool flashMode = false;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(0);

  kbox.setup();
  kbox.getNeopixels().setPixelColor(0, 0x00, 0x20, 0x00);
  kbox.getNeopixels().show();
  esp_init();
  esp_reboot_in_program();
}

void loop() {
  digitalWrite(led_pin, 1);
  uint8_t buffer[4096];

  // On most ESP, the RTS line is used to reset the module
  // DTR is used to boot in flash - or - run program
  if (Serial.rts() && !flashMode) {
    kbox.getNeopixels().setPixelColor(0, 0x20, 0x00, 0x00);
    kbox.getNeopixels().show();
    esp_reboot_in_flasher();
    flashMode = true;
  }

  if (Serial.available()) {
    digitalWrite(led_pin, 0);
    int read = Serial.readBytes((char*)buffer, sizeof(buffer));
    Serial1.write(buffer, read);
  }

  if (Serial1.available()) {
    digitalWrite(led_pin, 0);
    int read = Serial1.readBytes(buffer, sizeof(buffer));
    Serial.write(buffer, read);
  }
  // Fill up buffers a bit.
  delay(1);
}
