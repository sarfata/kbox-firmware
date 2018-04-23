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

#include "common/ui/GC.h"
#include "common/os/Task.h"
#include "common/signalk/SKNMEAOutput.h"
#include "common/signalk/SKNMEA2000Output.h"
#include "common/signalk/SKHub.h"
#include "common/signalk/SKSubscriber.h"
#include "common/comms/Kommand.h"
#include "common/comms/SlipStream.h"
#include "common/comms/KommandHandlerPing.h"
#include "host/comms/KommandHandlerWiFiLog.h"
#include "host/comms/KommandHandlerWiFiStatus.h"
#include "host/config/WiFiConfig.h"

/**
 * Manages connection to the ESP module.
 */
class WiFiService : public Task, public SKSubscriber,
                    public SKNMEAOutput, public SKNMEA2000Output,
                    private WiFiStatusObserver {
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
    void updateReceived(const SKUpdate&) override;

    bool write(const SKNMEASentence& s) override;
    bool write(const tN2kMsg& m) override;

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

