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

void WiFiTask::setup() {
  WiFiSerial.begin(115200);
  WiFiSerial.setTimeout(0);

  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_program, 0);
  digitalWrite(wifi_rst, 0);

  pinMode(wifi_enable, OUTPUT);
  pinMode(wifi_program, OUTPUT);
  if (wifi_cs > 0) {
    pinMode(wifi_cs, OUTPUT);
    digitalWrite(wifi_cs, 0);
  }
  pinMode(wifi_rst, OUTPUT);

  // To boot from Flash, we need
  // GPIO0 (wifi_program) HIGH and GPIO2 HIGH (pulled up via a resistor)
  // GPIO15 (wifi_cs) LOW
  // Then we need RST LOW and we set ENABLE HIGH
  digitalWrite(wifi_program, HIGH);

  // Make sure ESP got the memo
  delay(10);

  DEBUG("booting ESP!");

  // release reset pin and boot!
  digitalWrite(wifi_enable, 1);
  digitalWrite(wifi_rst, 1);
}

void WiFiTask::loop() {
  char buf[100];
  while (WiFiSerial.available() > 0) {
    int read = WiFiSerial.readBytes(buf, sizeof(buf) - 1);
    buf[read] = 0;
    DEBUG("WiFi: %s", buf);
  }
}

