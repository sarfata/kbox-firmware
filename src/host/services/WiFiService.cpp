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

#include "WiFiService.h"

#include <KBoxLogging.h>
#include <KBoxHardware.h>
#include <Seasmart.h>
#include "common/signalk/SKNMEAConverter.h"
#include "common/signalk/SKJSONVisitor.h"
#include "common/stats/KBoxMetrics.h"

WiFiService::WiFiService(const WiFiConfig &config, SKHub &skHub, GC &gc) :
  Task("WiFi"), _config(config), _hub(skHub), _slip(WiFiSerial, 2048),
  _wifiStatusHandler(*this), _espState(ESPState::ESPStarting)
{
  // We will need gc at some point to be able to take screenshot
}

void WiFiService::setup() {
  KBox.espInit();
  WiFiSerial.setTimeout(0);
  if (_config.enabled) {
    DEBUG("booting ESP!");
    KBox.espRebootInProgram();
  }

  _hub.subscribe(this);
}

void WiFiService::loop() {
  if (_slip.available()) {
    uint8_t *frame;
    size_t len = _slip.peekFrame(&frame);

    KommandReader kr = KommandReader(frame, len);

    KommandHandler *handlers[] = { &_pingHandler, &_wifiLogHandler,
                                   &_wifiStatusHandler, 0 };
    if (KommandHandler::handleKommandWithHandlers(handlers, kr, _slip)) {
      KBoxMetrics.event(KBoxEventWiFiValidKommand);
    }
    else {
      KBoxMetrics.event(KBoxEventWiFiInvalidKommand);
      ERROR("Invalid command received from WiFi module (id=%i size=%i)", kr.getKommandIdentifier(), kr.dataSize());

      // This is most likely a boot or reboot message from the ESP module.
      // Print it line by line, otherwise the message will be truncated.

      unsigned int index = 0;
      uint8_t *currentLine = frame;
      while (index < len) {
        // Remove \r to avoid extra blank lines in logs
        if (frame[index] == '\r') {
          frame[index] = '\0';
        }
        if (frame[index] == '\n') {
          frame[index] = '\0';
          ERROR("> %s", currentLine);

          currentLine = frame + index + 1;
        }
        index++;
      }

      frame[len-1] = 0;
      ERROR("> %s", currentLine);
    }

    _slip.readFrame(0, 0);
  }
}

void WiFiService::updateReceived(const SKUpdate& u) {
  /* This is where we convert the data in SignalK format that floats inside KBox
   * to messages that will be sent to WiFi clients.
   */

  if (!_config.enabled) {
    return;
  }

  SKNMEAConverter nmeaConverter(_config.nmeaConverter);
  // The converter will call this->write(NMEASentence) for every generated sentence
  nmeaConverter.convert(u, *this);

  // Now send in JSON format
  StaticJsonBuffer<1024> jsonBuffer;
  SKJSONVisitor jsonVisitor(_config.vesselURN, jsonBuffer);
  JsonObject &jsonData = jsonVisitor.processUpdate(u);
  FixedSizeKommand<1024> k(KommandSKData);
  jsonData.printTo(k);
  k.write(0);
  _slip.writeFrame(k.getBytes(), k.getSize());
}

bool WiFiService::write(const SKNMEASentence& sentence) {
  // NMEA Sentences should always be 82 bytes or less
  FixedSizeKommand<100> k(KommandNMEASentence);
  k.appendNullTerminatedString(sentence.c_str());
  _slip.writeFrame(k.getBytes(), k.getSize());
  return true;
}

bool WiFiService::write(const tN2kMsg& msg) {
  // PCDIN sentences should have a similar size as NMEA sentences.
  FixedSizeKommand<500> k(KommandNMEASentence);

  if (msg.DataLen > 500) {
    return false;
  }

  char pcdin[30 + msg.DataLen * 2];
  if (N2kToSeasmart(msg, millis(), pcdin, sizeof(pcdin)) < 500) {
    k.appendNullTerminatedString(pcdin);
    _slip.writeFrame(k.getBytes(), k.getSize());
    return true;
  } else {
    return false;
  }
}

void WiFiService::wiFiStatusUpdated(const ESPState &state, uint16_t dhcpClients,
                                    uint16_t tcpClients, uint16_t signalkClients,
                                    const IPAddress &ipAddress) {
  _espState = state;
  _clientAddress = ipAddress;
  _dhcpClients = dhcpClients;

  switch (state) {
    case ESPState::ESPStarting:
    case ESPState::ESPReady:
      sendConfiguration();
      break;

    case ESPState::ESPConfigured:
      _espState = state;
      break;
  }
}

const String WiFiService::clientInterfaceStatus() const {
  if (_config.client.enabled) {
    IPAddress ip = _clientAddress;
    if (static_cast<uint32_t >(ip) != 0) {
      return "connected";
    }
    else {
      return "connecting";
    }
  }
  return "(disabled)";
}

const IPAddress WiFiService::clientInterfaceIP() const {
  if (_config.client.enabled) {
    return _clientAddress;
  }
  else {
    return IPAddress((uint32_t)0);
  }
}

const String WiFiService::accessPointInterfaceStatus() const {
  if (_config.accessPoint.enabled) {
    return String(_config.accessPoint.ssid) + "(" + _dhcpClients + ")";
  }
  return "(disabled)";
}

const IPAddress WiFiService::accessPointInterfaceIP() const {
  if (_config.accessPoint.enabled) {
    return IPAddress(192, 168, 4, 1);
  }
  else {
    return IPAddress((uint32_t)0);
  }
}

void WiFiService::sendConfiguration() {
  FixedSizeKommand<1024> configFrame(KommandWiFiConfiguration);

  configFrame.append8(_config.accessPoint.enabled);
  configFrame.appendNullTerminatedString(_config.accessPoint.ssid);
  configFrame.appendNullTerminatedString(_config.accessPoint.password);

  configFrame.append8(_config.client.enabled);
  configFrame.appendNullTerminatedString(_config.client.ssid);
  configFrame.appendNullTerminatedString(_config.client.password);

  configFrame.appendNullTerminatedString(_config.vesselURN);

  _slip.writeFrame(configFrame.getBytes(), configFrame.getSize());
}
