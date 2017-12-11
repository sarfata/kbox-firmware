/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net

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
#include "common/config/KBoxConfig.h"
#include <KBoxLogging.h>
#include "signalk/SKUnits.h"


IMUMonitorPage::IMUMonitorPage(SKHub& hub, IMUService &imuService) : _imuService(imuService) {
  static const int col1 = 5;
  static const int col2 = 200;
  static const int row1 = 26;
  static const int row2 = 50;
  static const int row3 = 152;
  static const int row4 = 182;

  addLayer(new TextLayer(Point(col1, row1), Size(20, 20), "HDG ° Mag", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col2, row1), Size(20, 20), "Cal: Mag/Acc", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col1, row3), Size(20, 20), "Heel °", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col2, row3), Size(20, 20), "Pitch °", ColorWhite, ColorBlack, FontDefault));

  _hdgTL = new TextLayer(Point(col1, row2), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);
  _calTL = new TextLayer(Point(col2, row2), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);
  _rollTL  = new TextLayer(Point(col1, row4), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);
  _pitchTL = new TextLayer(Point(col2, row4), Size(20, 20), "--", ColorWhite, ColorBlack, FontLarge);

  addLayer(_hdgTL);
  addLayer(_calTL);
  addLayer(_rollTL);
  addLayer(_pitchTL);

  hub.subscribe(this);

}

bool IMUMonitorPage::processEvent(const ButtonEvent &be){

  // DEBUG("EventTypeButton: %i", be.clickType);
  if (be.clickType == ButtonEventTypeClick) {
    DEBUG("Button ButtonEventTypeClick !!!");
    // Change page on single click.
    return false;
  }
  if (be.clickType == ButtonEventTypeLongClick) {
    DEBUG("Button ButtonEventTypeLongClick !!!");
    // TODO: here will start the offset to zero calibration of heel & pitch
  }
  return true;
}

bool IMUMonitorPage::processEvent(const TickEvent &te){

  _imuService.getLastValues(_accelCalibration, _pitch, _roll, _magCalibration, _heading);
  //DEBUG("AccelCalibration: %i | MagCalibration: %i", _accelCalibration, _magCalibration);

  // TODO: Some damping for the display

  _hdgTL->setText(String( _heading, 1) + "°        ");
  _calTL->setText(String( _magCalibration) + "/" + String( _accelCalibration) + "   ");

  _pitchTL->setText(String( _pitch, 1) + "°     ");
  _rollTL->setText(String( _roll, 1) + "°     ");

  // Always show Hdg from IMU-sensor, but if the value is not trusted change color to Red
	if ((_magCalibration <= cfHdgMinCal)||(_accelCalibration <= cfHeelPitchMinCal)) {
    _hdgTL->setColor(ColorRed);
    _calTL->setColor(ColorRed);
  } else {
    _hdgTL->setColor(ColorWhite);
    _calTL->setColor(ColorWhite);
  };

  return true;
}

void IMUMonitorPage::updateReceived(const SKUpdate& up) {
  // may be needed for something....
}
