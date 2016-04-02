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

#include "esp8266.h"
#include "board.h"


void esp_init() {
  WiFiSerial.begin(115200);
  WiFiSerial.setTimeout(0);

  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_rst, 0);

  // wifi_program is gpio0, wifi_cs is gpio15
  // gpio2 is pulled up in hardware
  digitalWrite(wifi_program, 0);
  digitalWrite(wifi_cs, 0);

  pinMode(wifi_enable, OUTPUT);
  pinMode(wifi_rst, OUTPUT);
  pinMode(wifi_program, OUTPUT);

  if (wifi_cs > 0) {
    digitalWrite(wifi_cs, 0);
    pinMode(wifi_cs, OUTPUT);
  }

}

void esp_reboot_in_flasher() {
  digitalWrite(wifi_rst, 0);
  digitalWrite(wifi_program, 0);

  delay(10);

  // Boot the ESP module
  digitalWrite(wifi_enable, 1);
  digitalWrite(wifi_rst, 1);
}

void esp_reboot_in_program() {
  digitalWrite(wifi_rst, 0);
  digitalWrite(wifi_program, 1);

  delay(10);

  // Boot the ESP module
  digitalWrite(wifi_enable, 1);
  digitalWrite(wifi_rst, 1);
}


