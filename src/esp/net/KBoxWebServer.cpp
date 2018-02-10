/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net

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

#include "KBoxWebServer.h"

#include <ArduinoJson.h>

#include <KBoxLogging.h>
#include <ESPAsyncWebServer.h>
#include "common/version/KBoxVersion.h"

void handleRequestSignalK(AsyncWebServerRequest *request);

// FIXME: This should be a member of KBoxWebServer but...
// This library includes a class named LinkedList which conflicts with our
// LinkedList<> library. This is why we are hiding the include to the library
// here, away from other KBox code that uses our LinkedList ...
static AsyncWebServer webServer(80);
static AsyncWebSocket ws("/signalk/v1/stream");
static String s_vesselURN;
static int s_countClients;

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

KBoxWebServer::KBoxWebServer() {
}

void KBoxWebServer::setup() {
  s_countClients = 0;

  // attach AsyncWebSocket
  ws.onEvent(onEvent);
  webServer.addHandler(&ws);

  SPIFFS.begin();
  if (SPIFFS.exists("/index.html")) {
    DEBUG("Starting webserver with index.html existing on SPIFFS");
  }
  else {
    DEBUG("Starting webserver BUT index.html DOES NOT EXIST");
  }

  // respond to GET requests on URL /heap
  webServer.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  // Send the list of SignalK endpoints we support
  webServer.on("/signalk", HTTP_GET, handleRequestSignalK);

  // send a file when / is requested
  webServer.on("/", HTTP_ANY, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  // attach filesystem root at URL /fs
  webServer.serveStatic("/fs", SPIFFS, "/");

  webServer.onNotFound([](AsyncWebServerRequest *request) {
      DEBUG("404: %s", request->url().c_str());
      request->send(404, "text/plain", "No bounty for you here sailor. Keep looking. (404)");
  });

  webServer.begin();
}

void KBoxWebServer::publishSKUpdate(const char *message) {
  if (ws.enabled()) {
    ws.textAll(message);
  }
}

void KBoxWebServer::setVesselURN(const String &urn) {
  s_vesselURN = urn;
}

int KBoxWebServer::countClients() const {
  return s_countClients;
}

void handleRequestSignalK(AsyncWebServerRequest *request) {
  if (request->url() == "/signalk") {
    AsyncResponseStream *response = request->beginResponseStream("text/json");

    StaticJsonBuffer<500> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();

    JsonObject &endpoints = root.createNestedObject("endpoints");
    JsonObject &v1Endpoints = endpoints.createNestedObject("v1");
    v1Endpoints["version"] = "1.0.0";
    v1Endpoints["signalk-ws"] = "ws://" + request->host() + "/signalk/v1/stream";

    JsonObject &serverInfo = root.createNestedObject("server");
    serverInfo["id"] = "KBox";
    serverInfo["version"] = KBOX_VERSION;

    root.printTo(*response);
    request->send(response);
  }
  else {
    DEBUG("404: %s", request->url().c_str());
    request->send(404, "text/plain", "No bounty for you here sailor. Keep looking. (404)");
  }
}