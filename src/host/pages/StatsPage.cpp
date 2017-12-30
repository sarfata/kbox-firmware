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

#include <malloc.h>
#include <util/PrintableIPAddress.h>
#include "StatsPage.h"
#include "stats/KBoxMetrics.h"
#include "host/services/SDCardTask.h"
#include "host/services/WiFiService.h"

StatsPage::StatsPage() {
  loadView();
}

void StatsPage::loadView() {
  static const int margin = 2;
  static const int colWidth = 320/4;
  static const int col1 = margin;
  static const int col2 = col1 + colWidth;
  static const int col3 = 160;
  static const int col4 = col3 + colWidth;

  static const int rowHeight = 20;
  static const int row1 = margin;
  static const int row2 = row1 + rowHeight;
  static const int row3 = row2 + rowHeight;
  static const int row4 = row3 + rowHeight;
  static const int row5 = row4 + rowHeight;
  static const int row6 = row5 + rowHeight;
  static const int row7 = row6 + rowHeight;
  static const int row8 = row7 + rowHeight;

  static const int row11 = 240 - rowHeight - margin;
  static const int row10 = row11 - rowHeight;
  static const int row9 = row10 - rowHeight;

  //FIXME: the width below are incorrect. it might matter some day ...
  addLayer(new TextLayer(Point(col1, row1), Size(colWidth, rowHeight), "N1 Rx:"));
  addLayer(new TextLayer(Point(col1, row2), Size(colWidth, rowHeight), "N1 Rerr:"));
  addLayer(new TextLayer(Point(col1, row3), Size(colWidth, rowHeight), "N2 Rx:"));
  addLayer(new TextLayer(Point(col1, row4), Size(colWidth, rowHeight), "N2 Rerr:"));
  addLayer(new TextLayer(Point(col1, row5),
                         Size(colWidth, rowHeight), "N1 Tx:"));
  addLayer(new TextLayer(Point(col1, row6),
                         Size(colWidth, rowHeight), "N1 Terr:"));
  addLayer(new TextLayer(Point(col1, row7),
                         Size(colWidth, rowHeight), "N2 Tx:"));
  addLayer(new TextLayer(Point(col1, row8),
                         Size(colWidth, rowHeight), "N2 Terr:"));

  nmea1Rx = new TextLayer(Point(col2, row1), Size(colWidth, rowHeight), "0");
  nmea1Errors = new TextLayer(Point(col2, row2),
                              Size(colWidth, rowHeight), "0");
  nmea2Rx = new TextLayer(Point(col2, row3), Size(colWidth, rowHeight), "0");
  nmea2Errors = new TextLayer(Point(col2, row4),
                              Size(colWidth, rowHeight), "0");
  addLayer(nmea1Rx); addLayer(nmea1Errors); addLayer(nmea2Rx); addLayer(nmea2Errors);

  nmea1Tx = new TextLayer(Point(col2, row5),
                          Size(colWidth, rowHeight), "0");
  nmea1TxErrors = new TextLayer(Point(col2, row6),
                                Size(colWidth, rowHeight), "0");
  nmea2Tx = new TextLayer(Point(col2, row7),
                          Size(colWidth, rowHeight), "0");
  nmea2TxErrors = new TextLayer(Point(col2, row8),
                                Size(colWidth, rowHeight), "0");
  addLayer(nmea1Tx); addLayer(nmea1TxErrors);
  addLayer(nmea2Tx); addLayer(nmea2TxErrors);

  addLayer(new TextLayer(Point(col3, row1), Size(colWidth, rowHeight), "N2k Rx:"));
  addLayer(new TextLayer(Point(col3, row2), Size(colWidth, rowHeight), "N2k Tx:"));
  addLayer(new TextLayer(Point(col3, row3), Size(colWidth, rowHeight), "N2k err:"));

  canRx = new TextLayer(Point(col4, row1), Size(colWidth, rowHeight), "0");
  canTx = new TextLayer(Point(col4, row2), Size(colWidth, rowHeight), "0");
  canTxErrors = new TextLayer(Point(col4, row3), Size(colWidth, rowHeight), "0");
  addLayer(canRx); addLayer(canTx); addLayer(canTxErrors);

  // WiFi AP
  addLayer(new TextLayer(Point(col3, row5), Size(colWidth, rowHeight),
                         "WiFi-AP:"));
  addLayer(new TextLayer(Point(col3, row6), Size(colWidth, rowHeight), "IP:"));
  wifiAPStatus = new TextLayer(Point(col4 - 20, row5), Size(colWidth,
                                                            rowHeight), "");
  wifiAPIP = new TextLayer(Point(col4 - 40, row6), Size(colWidth, rowHeight), "");
  addLayer(wifiAPStatus); addLayer(wifiAPIP);

  // WiFi Client
  addLayer(new TextLayer(Point(col3, row7), Size(colWidth, rowHeight),
                         "WiFi-C:"));
  addLayer(new TextLayer(Point(col3, row8), Size(colWidth, rowHeight), "IP:"));
  wifiClientStatus = new TextLayer(Point(col4 - 20, row7), Size(colWidth,
                                                            rowHeight), "");
  wifiClientIP = new TextLayer(Point(col4 - 40, row8), Size(colWidth,
                                                           rowHeight), "");
  addLayer(wifiClientStatus); addLayer(wifiClientIP);

  addLayer(new TextLayer(Point(col1, row9), Size(colWidth, rowHeight), "Avg Loop:"));
  addLayer(new TextLayer(Point(col1, row10), Size(colWidth, rowHeight), "Used RAM:"));
  addLayer(new TextLayer(Point(col1, row11), Size(colWidth, rowHeight), "Free RAM:"));

  addLayer(new TextLayer(Point(col3, row9), Size(colWidth, rowHeight), "Log:"));
  addLayer(new TextLayer(Point(col3, row10), Size(colWidth, rowHeight), "Used:"));
  addLayer(new TextLayer(Point(col3, row11), Size(colWidth, rowHeight), "Free:"));

  avgLoopTime = new TextLayer(Point(col2, row9), Size(colWidth, rowHeight), "0");
  usedRam = new TextLayer(Point(col2, row10), Size(colWidth, rowHeight), "0");
  freeRam = new TextLayer(Point(col2, row11), Size(colWidth, rowHeight), "0");
  addLayer(usedRam); addLayer(freeRam); addLayer(avgLoopTime);

  logName = new TextLayer(Point(col4 - 40, row9), Size(colWidth, rowHeight),
                          "");
  logSize = new TextLayer(Point(col4, row10), Size(colWidth, rowHeight), "");
  freeSpace = new TextLayer(Point(col4, row11), Size(colWidth, rowHeight), "");
  addLayer(logName); addLayer(logSize); addLayer(freeSpace);
}

// https://forum.pjrc.com/threads/25676-Teensy-3-how-to-know-RAM-usage
// http://man7.org/linux/man-pages/man3/mallinfo.3.html
int getFreeRam() {
  return mallinfo().arena - mallinfo().keepcost;
}

int getUsedRam() {
  return mallinfo().uordblks;
}

bool StatsPage::processEvent(const TickEvent &e) {
  double avgLoopTimeUS = KBoxMetrics.averageMetric(KBoxMetricTaskManagerLoopUS);
  if (avgLoopTimeUS > 10000) {
    avgLoopTime->setText(String(avgLoopTimeUS / 1000, 2).append(" ms"));
  }
  else {
    avgLoopTime->setText(String(avgLoopTimeUS, 0).append(" us"));
  }

  nmea1Rx->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA1RX)));
  nmea1Errors->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA1RXError)));
  nmea1Tx->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA1TX)));
  nmea1TxErrors->setText(String(KBoxMetrics.countEvent
    (KBoxEventNMEA1TXOverflow)));
  nmea2Tx->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA2TX)));
  nmea2TxErrors->setText(String(KBoxMetrics.countEvent
    (KBoxEventNMEA2TXOverflow)));

  nmea2Rx->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA2RX)));
  nmea2Errors->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA2RXError)));

  canRx->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA2000MessageReceived)));
  canTx->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA2000MessageSent)));
  canTxErrors->setText(String(KBoxMetrics.countEvent(KBoxEventNMEA2000MessageSendError)));

  if (wifiService) {
    wifiAPStatus->setText(wifiService->accessPointInterfaceStatus());
    wifiClientStatus->setText(wifiService->clientInterfaceStatus());

    auto clientIP = PrintableIPAddress(wifiService->clientInterfaceIP());
    wifiClientIP->setText(clientIP.toString());

    auto apIP = PrintableIPAddress(wifiService->accessPointInterfaceIP());
    wifiAPIP->setText(apIP.toString());
  }

  freeRam->setText(String(getFreeRam()));
  usedRam->setText(String(getUsedRam()));

  if (sdcardTask) {
    if (sdcardTask->isLogging()) {
      logName->setText(sdcardTask->getLogFileName());
    }
    else {
      logName->setText("NOLOG");
      logName->setColor(ColorRed);
    }

    logSize->setText(formatDiskSize(sdcardTask->getLogSize()));
    freeSpace->setText(formatDiskSize(sdcardTask->getFreeSpace()));
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

  return String(size) + suffixes[suffix];
}

