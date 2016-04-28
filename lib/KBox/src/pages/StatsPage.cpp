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
#include "StatsPage.h"

StatsPage::StatsPage() : taskManager(0), reader1(0), reader2(0), nmea2000Task(0) {
  loadView();
}

void StatsPage::loadView() {
  static const int margin = 5;
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

  static const int row6 = 240 - rowHeight - margin;
  static const int row5 = row6 - rowHeight;

  //FIXME: the width below are incorrect. it might matter some day ...
  addLayer(new TextLayer(Point(col1, row1), Size(colWidth, rowHeight), "N1 Rx:"));
  addLayer(new TextLayer(Point(col1, row2), Size(colWidth, rowHeight), "N1 Rerr:"));
  addLayer(new TextLayer(Point(col1, row3), Size(colWidth, rowHeight), "N2 Rx:"));
  addLayer(new TextLayer(Point(col1, row4), Size(colWidth, rowHeight), "N2 Rerr:"));

  nmea1Rx = new TextLayer(Point(col2, row1), Size(colWidth, rowHeight), "0");
  nmea1Errors = new TextLayer(Point(col2, row2), Size(colWidth, rowHeight), "0");
  nmea2Rx = new TextLayer(Point(col2, row3), Size(colWidth, rowHeight), "0");
  nmea2Errors = new TextLayer(Point(col2, row4), Size(colWidth, rowHeight), "0");
  addLayer(nmea1Rx); addLayer(nmea1Errors); addLayer(nmea2Rx); addLayer(nmea2Errors);

  addLayer(new TextLayer(Point(col3, row1), Size(colWidth, rowHeight), "N2k Rx:"));
  addLayer(new TextLayer(Point(col3, row2), Size(colWidth, rowHeight), "N2k Tx:"));
  addLayer(new TextLayer(Point(col3, row3), Size(colWidth, rowHeight), "N2k Txerr:"));

  canRx = new TextLayer(Point(col4, row1), Size(colWidth, rowHeight), "0");
  canTx = new TextLayer(Point(col4, row2), Size(colWidth, rowHeight), "0");
  canTxErrors = new TextLayer(Point(col4, row3), Size(colWidth, rowHeight), "0");
  addLayer(canRx); addLayer(canTx); addLayer(canTxErrors);

  addLayer(new TextLayer(Point(col1, row5), Size(colWidth, rowHeight), "Used RAM:"));
  addLayer(new TextLayer(Point(col1, row6), Size(colWidth, rowHeight), "Free RAM:"));
  addLayer(new TextLayer(Point(col3, row6), Size(colWidth, rowHeight), "Avg Loop:"));

  usedRam = new TextLayer(Point(col2, row5), Size(colWidth, rowHeight), "0");
  freeRam = new TextLayer(Point(col2, row6), Size(colWidth, rowHeight), "0");
  avgLoopTime = new TextLayer(Point(col4, row6), Size(colWidth, rowHeight), "0");
  addLayer(usedRam); addLayer(freeRam); addLayer(avgLoopTime);
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
  if (taskManager) {
    avgLoopTime->setText(String(taskManager->getRunStat().avgTime() / 1000).append(" ms"));
  }
  if (reader1) {
    nmea1Rx->setText(String(reader1->getRxValidCounter()));
    nmea1Errors->setText(String(reader1->getRxErrorCounter()));
  }
  if (reader2) {
    nmea2Rx->setText(String(reader2->getRxValidCounter()));
    nmea2Errors->setText(String(reader2->getRxErrorCounter()));
  }
  if (nmea2000Task) {
    canRx->setText(String(nmea2000Task->getRxValidCounter()));
    canTx->setText(String(nmea2000Task->getTxValidCounter()));
    canTxErrors->setText(String(nmea2000Task->getTxErrors()));
  }

  freeRam->setText(String(getFreeRam()));
  usedRam->setText(String(getUsedRam()));

  return true;
}
