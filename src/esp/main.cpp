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

#include <KBoxLogging.h>
#include <KBoxLoggerStream.h>
#include <Adafruit_NeoPixel.h>
#include "NetServer.h"

uint8_t neopixel_pin = 4;

Adafruit_NeoPixel rgb = Adafruit_NeoPixel(1, neopixel_pin, NEO_GRB + NEO_KHZ800);
NetServer server(10110);

static const uint32_t connectedColor = rgb.Color(0x00, 0x40, 0x00);
static const uint32_t readyColor = rgb.Color(0x00, 0x00, 0x40);

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include "comms/SlipStream.h"
#include "comms/KommandHandler.h"
#include "comms/KommandHandlerPing.h"
#include "comms/KommandHandlerNMEA.h"
#include "stats/KBoxMetrics.h"
#include "net/KBoxWebServer.h"

#include "ESPDebugLogger.h"

SlipStream slip(Serial, 2048);
KommandHandlerPing pingHandler;
KommandHandlerNMEA nmeaHandler(server);
KBoxWebServer webServer;

void setup() {
  Serial1.begin(115200);
  KBoxLogging.setLogger(new ESPDebugLogger(slip));

  rgb.begin();
  rgb.setPixelColor(0, 0, 0, 0xff);
  rgb.show();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("KBox");

  Serial.begin(115200);
  Serial.setTimeout(0);
  Serial.setDebugOutput(true);

  webServer.setup();

  // This delay is important so that KBox can detect when firmware
  // upload are done and restart normal operation.
  delay(1000);

  DEBUG("Starting KBox WiFi module");
}

uint8_t buffer[1024];
size_t rxIndex = 0;

void loop() {
  static unsigned long int nextPrint = 0;
  if (millis() > nextPrint) {
    DEBUG("Still running ... %i connected clients - %i heap", server.clientsCount(), ESP.getFreeHeap());
    nextPrint = millis() + 500;
  }

  server.loop();

  while (slip.available()) {
    uint8_t *frame;
    size_t len = slip.peekFrame(&frame);

    KommandReader kr = KommandReader(frame, len);

    KommandHandler *handlers[] = { &pingHandler, &nmeaHandler, 0 };
    if (KommandHandler::handleKommandWithHandlers(handlers, kr, slip)) {
      KBoxMetrics.event(KBoxEventESPValidKommand);
    }
    else {
      KBoxMetrics.event(KBoxEventESPInvalidKommand);
    }
    slip.readFrame(0, 0);
  }

  if (server.clientsCount() > 0) {
    rgb.setPixelColor(0, connectedColor);
  }
  else {
    rgb.setPixelColor(0, readyColor);
  }
  rgb.show();
}

