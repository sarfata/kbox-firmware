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


