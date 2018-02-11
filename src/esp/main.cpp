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

static const uint32_t startingColor = rgb.Color(0x30, 0x10, 0x00);
static const uint32_t readyColor = rgb.Color(0x20, 0x20, 0x00);
static const uint32_t connectedColor = rgb.Color(0x00, 0x00, 0x40);
static const uint32_t clientsConnectedColor = rgb.Color(0x00, 0x40, 0x00);

#include <ESP8266WiFi.h>
#include <elapsedMillis.h>
#include "common/comms/SlipStream.h"
#include "common/comms/KommandHandler.h"
#include "common/comms/KommandHandlerPing.h"
#include "comms/KommandHandlerNMEA.h"
#include "comms/KommandHandlerSKData.h"
#include "comms/KommandHandlerWiFiConfiguration.h"
#include "common/comms/ESPState.h"
#include "common/stats/KBoxMetrics.h"
#include "net/KBoxWebServer.h"

#include "ESPDebugLogger.h"

SlipStream slip(Serial, 2048);
KommandHandlerPing pingHandler;
KommandHandlerNMEA nmeaHandler(server);
KBoxWebServer webServer;
KommandHandlerSKData skDataHandler(webServer);
KommandHandlerWiFiConfiguration wiFiConfigurationHandler;
ESPState espState;
WiFiEventHandler onGotIPHandler;
WiFiEventHandler onDhcpTimeoutHandler;
WiFiEventHandler onStationModeConnectedHandler;
WiFiEventHandler onStationModeDisconnectedHandler;

static void processSlipMessages();
static void reportStatus(ESPState state, uint16_t dhcpClients = 0,
                         uint16_t tcpClients = 0, uint16_t signalkClients = 0,
                         uint32_t ipAddress = 0);
static void configurationCallback(const WiFiConfiguration&);

static void onGotIP(const WiFiEventStationModeGotIP&);
static void onDHCPTimeout(void);
static void onStationModeConnected(const WiFiEventStationModeConnected&);
static void onStationModeDisconnected(const WiFiEventStationModeDisconnected&);

void setup() {
  Serial1.begin(115200);
  KBoxLogging.setLogger(new ESPDebugLogger(slip));

  rgb.begin();
  rgb.setPixelColor(0, startingColor);
  rgb.show();

  Serial.begin(115200);
  // We do want a timeout to make sure write() does not return
  // without having written everything.
  // FIXME: We should not use this!
  Serial.setTimeout(1000);

  // Turn on to get ESP debug messages (they will be intermixed with frames to
  // teensy)
  Serial.setDebugOutput(false);

  wiFiConfigurationHandler.setCallback(configurationCallback);

  onGotIPHandler = WiFi.onStationModeGotIP(onGotIP);
  onDhcpTimeoutHandler = WiFi.onStationModeDHCPTimeout(onDHCPTimeout);
  onStationModeConnectedHandler =
    WiFi.onStationModeConnected(onStationModeConnected);
  onStationModeDisconnectedHandler =
    WiFi.onStationModeDisconnected(onStationModeDisconnected);

  // Do not remember config automatically
  WiFi.persistent(false);

  // Configure our webserver
  webServer.setup();

  espState = ESPState::ESPStarting;

  // This delay is important so that KBox can detect when firmware
  // upload are done and restart normal operation.
  delay(1000);

  String resetInfo = ESP.getResetInfo();
  INFO("ESP8266 reboot reason: %s", resetInfo.c_str());
}

void loop() {
  static elapsedMillis lastMessageTimer = 0;

  processSlipMessages();
  server.loop();

  switch (espState) {
    case ESPState::ESPStarting:
      espState = ESPState::ESPReady;
      reportStatus(espState);
      lastMessageTimer = 0;
      rgb.setPixelColor(0, startingColor);
      rgb.show();

      break;

    case ESPState::ESPReady:
      // We should receive our configuration. If we don't let's re-send the
      // ready message.
      if (lastMessageTimer > 500) {
        reportStatus(espState);
        lastMessageTimer = 0;
      }
      rgb.setPixelColor(0, readyColor);
      break;

    case ESPState::ESPConfigured:
      // We are connected. Report status every 500ms.
      if (lastMessageTimer > 500) {
        DEBUG("Still running ... %i connected clients - %i heap",
              server.clientsCount() + webServer.countClients(), ESP.getFreeHeap());
        reportStatus(espState,
                     WiFi.softAPgetStationNum(),
                     server.clientsCount(),
                     webServer.countClients(),
                     static_cast<uint32_t>(WiFi.localIP()) );
        lastMessageTimer = 0;
      }

      if (server.clientsCount() > 0) {
        rgb.setPixelColor(0, clientsConnectedColor);
      }
      else {
        rgb.setPixelColor(0, connectedColor);
      }
      rgb.show();
      break;
  }
}

static void processSlipMessages() {
  while (slip.available()) {
    uint8_t *frame;
    size_t len = slip.peekFrame(&frame);

    KommandReader kr = KommandReader(frame, len);

    KommandHandler *handlers[] = { &pingHandler, &nmeaHandler,
                                   &skDataHandler, &wiFiConfigurationHandler,
                                   nullptr };
    if (KommandHandler::handleKommandWithHandlers(handlers, kr, slip)) {
      KBoxMetrics.event(KBoxEventESPValidKommand);
    }
    else {
      KBoxMetrics.event(KBoxEventESPInvalidKommand);
    }
    slip.readFrame(0, 0);
  }
}

static void configurationCallback(const WiFiConfiguration& config) {
  if (config.clientEnabled) {
    if (config.clientPassword.length() == 0) {
      if (!WiFi.begin(config.clientSSID.c_str())) {
        ERROR("Error connecting to %s", config.clientSSID.c_str());
      }
    }
    else {
      if (!WiFi.begin(config.clientSSID.c_str(),
                      config.clientPassword.c_str())) {
        ERROR("Error connecting to %s (with pass %s)",
              config.clientSSID.c_str(), config.clientPassword.c_str());
      }
    }
    WiFi.enableSTA(true);
  }
  else {
    WiFi.enableSTA(false);
  }

  if (config.accessPointEnabled) {
    if (config.accessPointPassword.length() == 0) {
      WiFi.softAP(config.accessPointSSID.c_str());
    }
    else {
      WiFi.softAP(config.accessPointSSID.c_str(),
                  config.accessPointPassword.c_str());
    }
    WiFi.enableAP(true);
  }
  else {
    WiFi.enableAP(false);
  }

  webServer.setVesselURN(config.vesselURN);

  espState = ESPState::ESPConfigured;
}

static void onGotIP(const WiFiEventStationModeGotIP& e) {
  DEBUG("DHCP Got IP Address!");
}

static void onDHCPTimeout(void) {
  DEBUG("DHCP Timeout");
}

static void onStationModeConnected(const WiFiEventStationModeConnected& e) {
  DEBUG("StationModeConnected");
}
static void onStationModeDisconnected(const WiFiEventStationModeDisconnected&
e) {
  DEBUG("StationModeDisconnected - reason=%i", e.reason);
}


static void reportStatus(ESPState state, uint16_t dhcpClients,
                         uint16_t tcpClients, uint16_t signalkClients,
                         uint32_t ipAddress) {
  FixedSizeKommand<12> kommand(KommandWiFiStatus);

  kommand.append16(static_cast<uint16_t>(state));
  kommand.append16(dhcpClients);
  kommand.append16(tcpClients);
  kommand.append16(signalkClients);
  kommand.append32(ipAddress);
  slip.writeFrame(kommand.getBytes(), kommand.getSize());
}
