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
#include "IMUMonitorPage.h"
#include "KBoxConfig.h"
#include <KBoxLogging.h>
#include "signalk/SKUnits.h"

IMUMonitorPage::IMUMonitorPage(SKHub& hub) {
  static const int col1 = 5;
  static const int col2 = 200;
  static const int row1 = 26;
  static const int row2 = 50;
  static const int row3 = 152;
  static const int row4 = 182;

  addLayer(new TextLayer(Point(col1, row1), Size(20, 20), "HDG ° Mag", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col2, row1), Size(20, 20), "Calibration", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col1, row3), Size(20, 20), "Heel °", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col2, row3), Size(20, 20), "Pitch °", ColorWhite, ColorBlack, FontDefault));

  hdgTL = new TextLayer(Point(col1, row2), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);
  calTL = new TextLayer(Point(col2, row2), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);
  heelTL  = new TextLayer(Point(col1, row4), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);
  pitchTL = new TextLayer(Point(col2, row4), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);

  addLayer(hdgTL);
  addLayer(calTL);
  addLayer(heelTL);
  addLayer(pitchTL);

  hub.subscribe(this);
}

void IMUMonitorPage::updateReceived(const SKUpdate& up) {


  if ( up.hasNavigationHeadingMagnetic() ) {
    const SKValue& vm = up.getNavigationHeadingMagnetic();
    DEBUG( "Heading Magnetic %f", SKRadToDeg( vm.getNumberValue() ) );
    hdgTL->setText(String( SKRadToDeg( vm.getNumberValue() ), 1) + "°        ");
    if ( magCal < cfIMU_MIN_CAL ) {
      hdgTL->setColor(ColorRed);
    } else {
      hdgTL->setColor(ColorWhite);
    };
  }

  if ( up.hasNavigationAttitude() ) {
    const SKValue& vm = up.getNavigationAttitude();
    pitchTL->setText(String( SKRadToDeg( vm.getAttitudeValue().pitch ), 1) + "°     ");
    heelTL->setText(String( SKRadToDeg( vm.getAttitudeValue().roll ), 1) + "°     ");
    //calTL->setText(String(vm.getCalibration()) + "   ");

  }

  //
  // setNavigationAttitude(SKTypeAttitude(/* roll */ SKDegToRad(roll), /* pitch */ SKDegToRad(pitch), /* yaw */ 0));
}
