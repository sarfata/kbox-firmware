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

#include <stdio.h>
#include "common/signalk/SKUpdate.h"
#include "BatteryMonitorPage.h"

BatteryMonitorPage::BatteryMonitorPage(SKHub& hub) {
  static const int col1 = 5;
  static const int col2 = 200;
  static const int row1 = 26;
  static const int row2 = 50;
  static const int row3 = 152;
  static const int row4 = 182;

  addLayer(new TextLayer(Point(col1, row1), Size(20, 20), "House Battery", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col2, row1), Size(20, 20), "House Current", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col1, row3), Size(20, 20), "Engine Battery", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col2, row3), Size(20, 20), "Supply Voltage", ColorWhite, ColorBlack, FontDefault));

  houseVoltage = new TextLayer(Point(col1, row2), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);
  houseCurrent = new TextLayer(Point(col2, row2), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);
  engineVoltage  = new TextLayer(Point(col1, row4), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);
  supplyVoltage = new TextLayer(Point(col2, row4), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);

  addLayer(houseVoltage);
  addLayer(houseCurrent);
  addLayer(engineVoltage);
  addLayer(supplyVoltage);

  hub.subscribe(this);
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

String BatteryMonitorPage::formatMeasurement(float measure, const char *unit) {
  // extra spaces at the end needed to clear previous value completely
  // (we use a non-fixed width font)
  char s[10];
  snprintf(s, sizeof(s), "%.1f %s  ", measure, unit);
  return String(s);
}

void BatteryMonitorPage::updateReceived(const SKUpdate& up) {
  if (up.hasElectricalBatteriesVoltage("house")) {
    const SKValue& vm = up.getElectricalBatteriesVoltage("house");
    houseVoltage->setText(formatMeasurement(vm.getNumberValue(), "V"));
    houseVoltage->setColor(colorForVoltage(vm.getNumberValue()));
  }
  if (up.hasElectricalBatteriesVoltage("engine")) {
    const SKValue& vm = up.getElectricalBatteriesVoltage("engine");
    engineVoltage->setText(formatMeasurement(vm.getNumberValue(), "V"));
    engineVoltage->setColor(colorForVoltage(vm.getNumberValue()));
  }
  if (up.hasElectricalBatteriesVoltage("kbox-supply")) {
    const SKValue& vm = up.getElectricalBatteriesVoltage("kbox-supply");
    supplyVoltage->setText(formatMeasurement(vm.getNumberValue(), "V"));
    supplyVoltage->setColor(colorForVoltage(vm.getNumberValue()));
  }
}
