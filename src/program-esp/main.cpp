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
