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

#include "StatsPage.h"

#include <KBoxHardware.h>
#include <common/time/WallClock.h>
#include "common/version/KBoxVersion.h"
#include "common/stats/KBoxMetrics.h"
#include "host/services/SDLoggingService.h"
#include "host/services/WiFiService.h"
#include "host/util/PrintableIPAddress.h"

StatsPage::StatsPage() {
  loadView();
}

void StatsPage::loadView() {
  static const int margin = 2;
  static const int colWidth = 320/4;
  static const int col1 = margin;
  static const int col2 = col1 + 60;
  static const int col3 = 160;
  static const int col4 = col3 + 60;

  static const int rowHeight = 20;
  static const int row1 = 40;
  static const int row2 = row1 + rowHeight;
  static const int row3 = row2 + rowHeight;
  static const int row4 = row3 + rowHeight;

  // Visual separation for wifi stuff
  static const int row5 = row4 + rowHeight + 8;
  static const int row6 = row5 + rowHeight;

  // Log info
  static const int row7 = row6 + rowHeight + 8;
  static const int row8 = row7 + rowHeight;

  // Bottom line with version
  static const int row9 = row8 + rowHeight + 8;

  dateTime = new TextLayer(Point(25, 0), Size(320-25, 20), "");
  dateTime->setFont(FontLarger);
  addLayer(dateTime);

  addLayer(new TextLayer(Point(col1, row1), Size(colWidth, rowHeight), "N1 Rx:"));
  addLayer(new TextLayer(Point(col1, row2), Size(colWidth, rowHeight), "N2 Rx:"));
  addLayer(new TextLayer(Point(col1, row3),
                         Size(colWidth, rowHeight), "N1 Tx:"));
  addLayer(new TextLayer(Point(col1, row4),
                         Size(colWidth, rowHeight), "N2 Tx:"));

  nmea1Rx = new TextLayer(Point(col2, row1), Size(colWidth, rowHeight), "0");
  nmea2Rx = new TextLayer(Point(col2, row2), Size(colWidth, rowHeight), "0");
  addLayer(nmea1Rx); addLayer(nmea2Rx);

  nmea1Tx = new TextLayer(Point(col2, row3), Size(colWidth, rowHeight), "0");
  nmea2Tx = new TextLayer(Point(col2, row4), Size(colWidth, rowHeight), "0");
  addLayer(nmea1Tx); addLayer(nmea2Tx);

  addLayer(new TextLayer(Point(col3, row1), Size(colWidth, rowHeight), "N2k Rx:"));
  addLayer(new TextLayer(Point(col3, row2), Size(colWidth, rowHeight), "N2k Tx:"));

  canRx = new TextLayer(Point(col4, row1), Size(colWidth, rowHeight), "0");
  canTx = new TextLayer(Point(col4, row2), Size(colWidth, rowHeight), "0");
  addLayer(canRx); addLayer(canTx);

  addLayer(new TextLayer(Point(col3, row3), Size(colWidth, rowHeight), "ESP Rx:"));
  addLayer(new TextLayer(Point(col3, row4), Size(colWidth, rowHeight), "ESP Tx:"));

  espRx = new TextLayer(Point(col4, row3), Size(colWidth, rowHeight), "0");
  espTx = new TextLayer(Point(col4, row4), Size(colWidth, rowHeight), "0");
  addLayer(espRx); addLayer(espTx);

  // WiFi AP
  wifiAPStatus = new TextLayer(Point(col1, row5), Size(colWidth, rowHeight), "");
  wifiAPIP = new TextLayer(Point(col1, row6), Size(colWidth, rowHeight), "");
  addLayer(wifiAPStatus); addLayer(wifiAPIP);

  // WiFi Client
  wifiClientStatus = new TextLayer(Point(col3, row5), Size(colWidth, rowHeight), "");
  wifiClientIP = new TextLayer(Point(col3, row6), Size(colWidth, rowHeight), "");
  addLayer(wifiClientStatus); addLayer(wifiClientIP);

  addLayer(new TextLayer(Point(col3, row7), Size(colWidth, rowHeight), "Avg Loop:"));
  addLayer(new TextLayer(Point(col3, row8), Size(colWidth, rowHeight), "RAM:"));

  logName = new TextLayer(Point(col1, row7), Size(320 - col1 - colWidth, rowHeight), "");
  logSize = new TextLayer(Point(col1, row8), Size(colWidth, rowHeight), "");
  addLayer(logName); addLayer(logSize);

  avgLoopTime = new TextLayer(Point(col4 + 20, row7), Size(colWidth, rowHeight), "0");
  usedRam = new TextLayer(Point(col4, row8), Size(colWidth, rowHeight), "0");
  addLayer(usedRam); addLayer(avgLoopTime);

  addLayer(new TextLayer(Point(125, row9), Size(col4 - col1, rowHeight), KBOX_VERSION));
}

// Return just "c" or "c (e)" if e > 0
static String formatCounterWithEventualError(uint32_t c, uint32_t e) {
  if (e == 0) {
    return String(c);
  }
  else {
    String m = c;
    m += " (";
    m += e;
    m += ")";
    return m;
  }
}

bool StatsPage::processEvent(const TickEvent &e) {
  // Update the time if a time is available
  SKTime now = wallClock.now();
  if (wallClock.isTimeSet()) {
    String timeDisplay = now.iso8601date();
    timeDisplay += " ";
    timeDisplay += now.iso8601extendedTime();

    dateTime->setColor(ColorWhite);
    dateTime->setText(timeDisplay);
  }
  else {
    dateTime->setColor(ColorRed);
    dateTime->setText(now.iso8601extendedTime());
  }

  nmea1Rx->setText(formatCounterWithEventualError(KBoxMetrics.countEvent(KBoxEventNMEA1RX),
                                                  KBoxMetrics.countEvent(KBoxEventNMEA1RXError)
                                                  + KBoxMetrics.countEvent(KBoxEventNMEA1RXOverflow)
                                                  + KBoxMetrics.countEvent(KBoxEventNMEA1RXBufferOverflow)));
  nmea2Rx->setText(formatCounterWithEventualError(KBoxMetrics.countEvent(KBoxEventNMEA2RX),
                                                  KBoxMetrics.countEvent(KBoxEventNMEA2RXError)
                                                  + KBoxMetrics.countEvent(KBoxEventNMEA2RXOverflow)
                                                  + KBoxMetrics.countEvent(KBoxEventNMEA2RXBufferOverflow)));
  nmea1Tx->setText(formatCounterWithEventualError(KBoxMetrics.countEvent(KBoxEventNMEA1TX),
                                                  KBoxMetrics.countEvent(KBoxEventNMEA1TXOverflow)));
  nmea2Tx->setText(formatCounterWithEventualError(KBoxMetrics.countEvent(KBoxEventNMEA2TX),
                                                  KBoxMetrics.countEvent(KBoxEventNMEA2TXOverflow)));


  canRx->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA2000MessageReceived)));
  canTx->setText(formatCounterWithEventualError(KBoxMetrics.countEvent(KBoxEventNMEA2000MessageSent),
                                                KBoxMetrics.countEvent(KBoxEventNMEA2000MessageSendError)));

  espRx->setText(formatCounterWithEventualError(KBoxMetrics.countEvent(KBoxEventWiFiRxValidKommand),
                                                KBoxMetrics.countEvent(KBoxEventWiFiRxErrorFrame)));
  espTx->setText(formatCounterWithEventualError(KBoxMetrics.countEvent(KBoxEventWiFiTxFrame),
                                                KBoxMetrics.countEvent(KBoxEventWiFiRxInvalidKommand)));

  if (wifiService) {
    if (wifiService->accessPointEnabled()) {
      wifiAPStatus->setText(wifiService->accessPointNetworkName() + " (" + wifiService->accessPointClients() + ")");
      auto apIP = PrintableIPAddress(wifiService->accessPointInterfaceIP());
      wifiAPIP->setText(apIP.toString());
    }
    else {
      wifiAPStatus->setText("Access Point Disabled");
    }

    if (wifiService->clientInterfaceEnabled()) {
      wifiClientStatus->setText(wifiService->clientInterfaceNetworkName());
      if (wifiService->clientInterfaceConnected()) {
        auto clientIP = PrintableIPAddress(wifiService->clientInterfaceIP());
        wifiClientIP->setColor(ColorWhite);
        wifiClientIP->setText(clientIP.toString());
      } else {
        wifiClientIP->setColor(ColorOrange);
        wifiClientIP->setText("Connecting");
      }
    }
    else {
      wifiClientStatus->setText("WiFi Client Disabled");
    }

  }

  if (sdcardTask) {
    if (sdcardTask->isLogging()) {
      logName->setText(sdcardTask->getLogFileName());
      logName->setColor(ColorWhite);
    }
    else {
      logName->setText("NOLOG");
      logName->setColor(ColorRed);
    }

    String logText = formatDiskSize(sdcardTask->getLogSize());
    logText += "/";
    logText += formatDiskSize(sdcardTask->getLogSize() + sdcardTask->getFreeSpace());

    logSize->setText(logText);
  }

  String ramText = String(KBox.getFreeRam());
  ramText += "/";
  ramText += KBox.getFreeRam() + KBox.getUsedRam();
  usedRam->setText(ramText);

  double avgLoopTimeUS = KBoxMetrics.averageMetric(KBoxMetricTaskManagerLoopUS);
  if (avgLoopTimeUS > 10000) {
    avgLoopTime->setText(String(avgLoopTimeUS / 1000, 2).append(" ms"));
  }
  else {
    avgLoopTime->setText(String(avgLoopTimeUS, 0).append(" us"));
  }

  return true;
}

String StatsPage::formatDiskSize(uint64_t intSize) {
  static const String suffixes[] = { "B", "kB", "MB", "GB" };

  float size = intSize;
  unsigned int suffix = 0;
  while (size > 8000 && suffix < sizeof(suffixes)/sizeof(suffixes[0])) {
    size /= 1024;
    suffix++;
  }

  // Do not display decimals for bytes
  if (suffix == 0) {
    return String(static_cast<int>(size)) + suffixes[suffix];
  }
  else {
    return String(size) + suffixes[suffix];
  }
}

