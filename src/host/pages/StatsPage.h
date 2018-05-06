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

#include "common/ui/Page.h"
#include "common/ui/TextLayer.h"

class SDLoggingService;
class WiFiService;

class StatsPage : public Page {
  private:
    TextLayer *dateTime;
    TextLayer *nmea1Rx, *nmea2Rx;
    TextLayer *nmea1Tx, *nmea2Tx;
    TextLayer *canRx, *canTx;
    TextLayer *wifiAPStatus, *wifiAPIP;
    TextLayer *wifiClientStatus, *wifiClientIP;
    TextLayer *usedRam, *freeRam, *avgLoopTime;
    TextLayer *logName, *logSize, *freeSpace;

    SDLoggingService *sdcardTask = 0;
    const WiFiService *wifiService = 0;

    void loadView();
    String formatDiskSize(uint64_t intSize);

  public:
    StatsPage();
    bool processEvent(const TickEvent &e);

    void setSDLoggingService(SDLoggingService *t) {
      sdcardTask = t;
    };

    void setWiFiService(const WiFiService *s) {
      wifiService = s;
    }
};
