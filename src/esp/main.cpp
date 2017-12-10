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
#include <Adafruit_NeoPixel.h>
#include "ESPDebug.h"
#include "NetServer.h"

uint8_t neopixel_pin = 4;

Adafruit_NeoPixel rgb = Adafruit_NeoPixel(1, neopixel_pin, NEO_GRB + NEO_KHZ800);
NetServer server(10110);

static const uint32_t connectedColor = rgb.Color(0x00, 0xff, 0x00);
static const uint32_t readyColor = rgb.Color(0x00, 0x00, 0xff);
//RES_MOD_8_2_17 add HeronRpi static parameters
const char*  ssid = "HeronRpi";
const char*  password = "password";

#include <Arduino.h>
#include <ESP8266WiFi.h>
//RREMOD_8_4_17 mod wifi to connect to HeronRpi network
void setup() {
  DEBUG_INIT();

  rgb.begin();
  rgb.setPixelColor(0, 0, 0, 0xff);
  rgb.show();
  //RES_MOD_8_2_17 comment out softAP setup - 2 lines use station class
  //WiFi.mode(WIFI_AP);
  //WiFi.softAP("KBox");

  Serial.begin(115200);
  Serial.setTimeout(0);
  Serial.setDebugOutput(true);

  // This delay is important so that KBox can detect when firmware
  // upload are done and restart normal operation.
  delay(1000);

  //DEBUG("Starting KBox WiFi module");

  //RES_MOD_8_2_17 use wifi.begin instead -up to } is mine
  DEBUG("Starting Kbox WiFi to connect to HeronRpi");
  WiFi.begin(ssid, password);
  DEBUG("Connecting to", ssid);
  int i = 0;
  //RES_MOD_8_2_17 wait for the wifi to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    DEBUG("waiting to connect", ++i);

  }
  //RES_MOD_9_6_17  serial print when connected
  DEBUG("Connection established");
  DEBUG("Connected to  %s", WiFi.localIP().toString().c_str());

  //RES_MOD_8_2_17 end of my add
}

uint8_t buffer[1024];
size_t rxIndex = 0;

void loop() {
  static unsigned long int nextPrint = 0;
  if (millis() > nextPrint) {
    DEBUG("Still running ... %i connected clients - %i heap - IP: %s", server.clientsCount(), ESP.getFreeHeap(), WiFi.localIP().toString().c_str());
    nextPrint = millis() + 500;
  }

  server.loop();

  while (Serial.available()) {
    buffer[rxIndex++] = Serial.read();

    if (buffer[rxIndex - 1] == '\n') {
      server.writeAll(buffer, rxIndex);
      rxIndex = 0;
    }
    if (rxIndex == sizeof(buffer)) {
      DEBUG("Buffer overflow. Clearing buffer.");
      rxIndex = 0;
    }
  }

  if (server.clientsCount() > 0) {
    rgb.setPixelColor(0, connectedColor);
  }
  else {
    rgb.setPixelColor(0, readyColor);
  }
  rgb.show();
}
