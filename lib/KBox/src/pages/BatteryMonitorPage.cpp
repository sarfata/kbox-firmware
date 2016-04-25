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

#include "BatteryMonitorPage.h"
#include <stdio.h>

void BatteryMonitorPage::willAppear() {
  DEBUG("view will appear!");
  needsFullPainting = needsPainting = true;
}

Color BatteryMonitorPage::colorForVoltage(float v) {
  if (v < 0.1) {
    return ColorWhite;
  }
  if (v < 11.8) {
    return ColorRed;
  }
  if (v < 13.5) {
    return ColorGreen;
  }
  return ColorBlue;
}

const char *BatteryMonitorPage::formatMeasurement(float measure, const char *unit, bool valid) {
  static char text[20];

  if (valid) {
    // extra spaces at the end needed to clear previous value completely
    // (we use a non-fixed width font)
    snprintf(text, sizeof(text), "%4.1f %s  ", measure, unit);
  }
  else {
    snprintf(text, sizeof(text), "%s", "--");
  }
  return text;
}

void BatteryMonitorPage::paint(GC &gc) {
  static const int col1 = 5;
  static const int col2 = 200;
  static const int row1 = 26;
  static const int row2 = 50;
  static const int row3 = 152;
  static const int row4 = 182;

  static const int margin = 5;

  if (needsFullPainting) {
    DEBUG("Doing full paint");
    gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlack);

    // Horizontal line
    //gc.drawLine(Point(0, row3 - margin), Point(320, row3 - margin), ColorWhite);
    // Vertical line
    //gc.drawLine(Point(col2 - margin, 0), Point(col2 - margin, 240), ColorWhite);

    gc.drawText(Point(col1, row1), FontDefault, ColorWhite, ColorBlack, "House Battery");
    gc.drawText(Point(col2, row1), FontDefault, ColorWhite, ColorBlack, "House Current");
    gc.drawText(Point(col1, row3), FontDefault, ColorWhite, ColorBlack, "Engine Battery");
    gc.drawText(Point(col2, row3), FontDefault, ColorWhite, ColorBlack, "Supply Voltage");

    needsFullPainting = false;
  }

  if (! needsPainting) {
    return;
  }

  /* Display house batter voltage */
  gc.drawText(Point(col1, row2), FontLarge, colorForVoltage(houseVoltage), ColorBlack, formatMeasurement(houseVoltage, "V", displayHouseVoltage));

  /* Display house current */
  gc.drawText(Point(col2, row2), FontLarge, colorForVoltage(houseCurrent), ColorBlack, formatMeasurement(houseCurrent, "A", displayHouseCurrent));

  /* Display engine battery voltage */
  gc.drawText(Point(col1, row4), FontLarge, colorForVoltage(starterVoltage), ColorBlack, formatMeasurement(starterVoltage, "V", displayStarterVoltage));

  /* Display nmea2000 bus voltage */
  gc.drawText(Point(col2, row4), FontLarge, colorForVoltage(supplyVoltage), ColorBlack, formatMeasurement(supplyVoltage, "V"));

  needsPainting = false;
}


void BatteryMonitorPage::processMessage(const KMessage &message) {
  if (message.getMessageType() == KMessageType::VoltageMeasurement) {
    const VoltageMeasurement *vm = static_cast<const VoltageMeasurement*>(&message);
    // Maybe a bit "too" hardcoded?
    if (vm->getLabel() == "house") {
      displayHouseVoltage = true;
      houseVoltage = vm->getVoltage();
      needsPainting = true;
    }
    if (vm->getLabel() == "supply") {
      supplyVoltage = vm->getVoltage();
      needsPainting = true;
    }
    if (vm->getLabel() == "starter") {
      displayStarterVoltage = true;
      starterVoltage = vm->getVoltage();
      needsPainting = true;
    }
  }
}

