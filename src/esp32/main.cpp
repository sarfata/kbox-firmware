/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

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
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <elapsedMillis.h>
#include <ArduinoJson.h>
#include <Seasmart.h>
#include <esp_partition.h>
#include <SPIFFSEditor.h>
#include <SPIFFS.h>
#include <SD.h>
#include "NMEA2000Task.h"
#include "SDLogging.h"
#include "NMEAServer.h"

AsyncWebServer webServer(80);
AsyncWebSocket ws("/signalk/v1/stream");
NMEA2000Task nmea2000Task;
SDLogging sdLoggingTask;
NMEAServer nmeaServer;

static int s_countClients = 0;
static const char *s_vesselURN = "myVesselURN";

// FIXME: We are going to have some threading problems here because the onEvent
// will be called on a network thread and KBoxLogging is not thread-safe
void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type,
             void * arg, uint8_t *data, size_t len){
  if (type == WS_EVT_CONNECT) {
    DEBUG("%u: New connection from %s to %s", client->id(), client->remoteIP().toString().c_str(), server->url() );
    s_countClients++;

    StaticJsonBuffer<500> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["version"] = "1.0.2";
    //root["timestamp"] = "";
    root["self"] =  s_vesselURN;

    char buffer[256];
    root.printTo(buffer, sizeof(buffer));

    client->printf(buffer);
  }
  else if (type == WS_EVT_DISCONNECT) {
    DEBUG("%u: Disconnected", client->id());
    s_countClients--;
  }
  else if (type == WS_EVT_ERROR) {
    DEBUG("%u: Error", client->id());
    s_countClients--;
  }
  else if (type == WS_EVT_PONG) {
    DEBUG("%u: PONG", client->id());
  }
  else if (type == WS_EVT_DATA) {
    DEBUG("%u: DATA", client->id());
  }
  else {
    DEBUG("%u: Un-handled event with type=%i", client->id(), type);
  }
}

class WebSocketSender : public NMEA2000WriterInterface {
  public:
    void write(const tN2kMsg &msg) override {
      char buf[400];
      if (N2kToSeasmart(msg, xTaskGetTickCount(), buf, sizeof(buf)) == 0) {
        ERROR("n2k conversion error");
      }
      else {
        ws.textAll(buf);
      }
    }
};

void setup() {
  Serial.begin(115200);
  KBoxLogging.setLogger(new KBoxLoggerStream(Serial));

  DEBUG("KBox Starting");

  WiFi.onEvent([](WiFiEvent_t event){
    DEBUG("WiFi Ready");
  }, WiFiEvent_t::SYSTEM_EVENT_WIFI_READY);

  WiFi.onEvent([](WiFiEvent_t event){
      DEBUG("WiFi Station Start");
  }, WiFiEvent_t::SYSTEM_EVENT_STA_START);

  WiFi.onEvent([](WiFiEvent_t event){
      DEBUG("WiFi Station Connected");
  }, WiFiEvent_t::SYSTEM_EVENT_STA_CONNECTED);

  WiFi.onEvent([](WiFiEvent_t event){
      DEBUG("WiFi Station Connected");
  }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

  WiFi.onEvent([](WiFiEvent_t event){
      DEBUG("WiFi Station Got IP");
  }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);

  WiFi.begin("DorothyPacke-PC-Wireless", "PalmSpringsWireless");


  webServer.onNotFound([](AsyncWebServerRequest *request) {
      DEBUG("404: %s", request->url().c_str());
      request->send(404, "text/plain", "No bounty for you here sailor. Keep looking. (404)");
  });
  // attach AsyncWebSocket
  ws.onEvent(onEvent);
  webServer.addHandler(&ws);

  SPIFFS.begin();
  sdLoggingTask.initializeCard();

  webServer.addHandler(new SPIFFSEditor(SD, "kbox", "test"));
  webServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  webServer.begin();


  nmea2000Task.setup();
  nmea2000Task.addWriter(&sdLoggingTask);

  WebSocketSender *wsSender = new WebSocketSender();
  nmea2000Task.addWriter(wsSender);

  nmea2000Task.addWriter(&nmeaServer);
  nmeaServer.start();


  size_t ul;
  esp_partition_iterator_t _mypartiterator;
  const esp_partition_t *_mypart;
  ul = spi_flash_get_chip_size(); Serial.print("Flash chip size: "); Serial.println(ul);
  Serial.println("Partiton table:");
  _mypartiterator = esp_partition_find(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, NULL);
  if (_mypartiterator) {
    do {
      _mypart = esp_partition_get(_mypartiterator);
      printf("%x - %x - %x - %x - %s - %i\n", _mypart->type, _mypart->subtype, _mypart->address, _mypart->size, _mypart->label, _mypart->encrypted);
    } while ((_mypartiterator = esp_partition_next(_mypartiterator)));
  }
  esp_partition_iterator_release(_mypartiterator);
  _mypartiterator = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);
  if (_mypartiterator) {
    do {
      _mypart = esp_partition_get(_mypartiterator);
      printf("%x - %x - %x - %x - %s - %i\n", _mypart->type, _mypart->subtype, _mypart->address, _mypart->size, _mypart->label, _mypart->encrypted);
    } while ((_mypartiterator = esp_partition_next(_mypartiterator)));
  }
  esp_partition_iterator_release(_mypartiterator);
}

void loop() {
  static elapsedMillis lastLogMessage = 0;
  if (lastLogMessage > 1000) {
    DEBUG("KBox Running");
    lastLogMessage = 0;
  }

  static elapsedMillis lastSDCardWrite = 0;
  if (lastSDCardWrite > 2000) {
    sdLoggingTask.loop();
    lastSDCardWrite = 0;
  }
  nmea2000Task.loop();
}