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

#include "WiFiTask.h"
#include "../drivers/board.h"
#include "../drivers/esp8266.h"

void WiFiTask::setup() {
  esp_init();
  DEBUG("booting ESP!");
  esp_reboot_in_program();
}

void WiFiTask::loop() {
  char buf[100];
  while (WiFiSerial.available() > 0) {
    int read = WiFiSerial.readBytes(buf, sizeof(buf) - 1);
    buf[read] = 0;
    DEBUG("WiFi: %s", buf);
  }
}

void WiFiTask::processMessage(const KMessage &m) {
    WiFiSerial.println(m.toString());
    DEBUG("Sending %s", m.toString().c_str());
}


