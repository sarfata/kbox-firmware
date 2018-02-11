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
#pragma once

#include "ui/GC.h"
#include "os/Task.h"
#include "signalk/KMessage.h"
#include "signalk/SKNMEAOutput.h"
#include "signalk/SKHub.h"
#include "signalk/SKSubscriber.h"
#include "comms/Kommand.h"
#include "comms/SlipStream.h"
#include "comms/KommandHandlerPing.h"
#include "comms/KommandHandlerWiFiLog.h"
#include "comms/KommandHandlerWiFiStatus.h"
#include "host/config/WiFiConfig.h"

/**
 * Manages connection to the ESP module.
 */
class WiFiService : public Task, public KReceiver, public SKSubscriber,
                    private SKNMEAOutput, private WiFiStatusObserver {
  private:
    const WiFiConfig &_config;
    SKHub &_hub;
    SlipStream _slip;
    KommandHandlerPing _pingHandler;
    KommandHandlerWiFiLog _wifiLogHandler;
    KommandHandlerWiFiStatus _wifiStatusHandler;
    ESPState _espState;

    IPAddress _clientAddress;
    uint16_t _dhcpClients;

  public:
    WiFiService(const WiFiConfig &config, SKHub &skHub, GC &gc);

    void setup();
    void loop();
    void processMessage(const KMessage &m) override;
    void updateReceived(const SKUpdate&) override;

    bool write(const SKNMEASentence& s) override;

    const String clientInterfaceStatus() const;
    const IPAddress clientInterfaceIP() const;
    const String accessPointInterfaceStatus() const;
    const IPAddress accessPointInterfaceIP() const;

  private:
    // WiFiStatusObserver
    void wiFiStatusUpdated(const ESPState &state, uint16_t dhcpClients,
                           uint16_t tcpClients, uint16_t signalkClients,
                           const IPAddress &ipAddress) override;

    void sendConfiguration();
};

