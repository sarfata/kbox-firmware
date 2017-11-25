/*
  * Project:  KBox
  * Purpose:  IMU Monitor Page for HDG, Heel & Pitch (+ Cal planned)
  * Author:   (c) Ronnie Zeiller, 2017
  * KBox:     (c) 2017 Thomas Sarlandie thomas@sarlandie.net

  The MIT License

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
#include <KBoxLogging.h>
#include "signalk/SKUnits.h"
#include "services/IMUService.h"
#include "common/signalk/SKUpdateStatic.h"
#include "common/signalk/SKUnits.h"


IMUMonitorPage::IMUMonitorPage(SKHub& hub) {
  _IMU_HDGFiltered    = 361;
  _IMU_PitchFiltered  = 181;
  _IMU_HeelFiltered   = 181;

  // Damping values 1.....100 --> 0.5 no filter ......0.005 max filter
  // for higher frequencies even more
  // TODO: make KBox setting for damping values 1.......100

  // auto damping factored by the IMUServiceInterval which is
  // probably between 50 and 100 (20 - 10 Hz)
  // TODO: set and get IMUServiceInterval for automatic modus -> #define IMUServiceInterval ??

  _dampingHdg = _IMUServiceInterval / 1400.000; // 0.05
  if (_dampingHdg>0.5) _dampingHdg=0.5;


  // Heel and Pitch need smaller damping
  _dampingHeelPitch = _IMUServiceInterval / 700.000; // 0.1 .... 0.1;
  if (_dampingHeelPitch>0.5) _dampingHeelPitch=0.5;

  INFO( "HDG Damping factor taken for IIR-filter: %f", _dampingHdg );
  INFO( "Heel/Pitch Damping factor taken for IIR-filter: %f", _dampingHeelPitch );

  // set filter type
  IMU_HdgFilter.setType(IIRFILTER_TYPE_DEG);
  IMU_HdgFilter.setFC(_dampingHdg);
  IMU_HeelFilter.setType(IIRFILTER_TYPE_LINEAR);
  IMU_HeelFilter.setFC(_dampingHeelPitch);
  IMU_PitchFilter.setType(IIRFILTER_TYPE_LINEAR);
  IMU_PitchFilter.setFC(_dampingHeelPitch);


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

  // No Updates coming when Calib below 3!!!
  // Aproach is for trusted values only
  // Disadvantage is, that the display is stuck to the last value!
  // TODO: get Calibration from IMU Service and show on monitor (may be red = warning not trusted value)

  if ( up.hasNavigationHeadingMagnetic() ) {
    const SKValue& vm = up.getNavigationHeadingMagnetic();
    _IMU_HDGFiltered = IMU_HdgFilter.filter( round ( SKRadToDeg( vm.getNumberValue()) * 10) / 10.0 );
    //DEBUG("HDG: %f\n", round( SKRadToDeg( vm.getNumberValue()) * 10) / 10.0 );
  }

  if ( up.hasNavigationAttitude() ) {
    const SKValue& vm = up.getNavigationAttitude();
    _IMU_HeelFiltered = IMU_HeelFilter.filter( round( SKRadToDeg( vm.getAttitudeValue().roll ) * 10) / 10.0 );
    _IMU_PitchFiltered = IMU_PitchFilter.filter( round( SKRadToDeg( vm.getAttitudeValue().pitch )* 10) / 10.0 );
    //DEBUG("Heel: %f\n", round( SKRadToDeg( vm.getAttitudeValue().roll ) * 10) / 10.0);
    //DEBUG("Pitch: %f\n", round( SKRadToDeg( vm.getAttitudeValue().pitch )* 10) / 10.0);
  }


  // Display interval 400ms
  if (millis() > _nextPrint) {
    _nextPrint = millis() + 400;


    //DEBUG("%f\n", _IMU_HDGFiltered);
    //DEBUG("%f\n",_IMU_PitchFiltered);
    //DEBUG("%f\n",_IMU_HeelFiltered);
    if ( _IMU_HDGFiltered < 361 && _IMU_HDGFiltered >= 0 )
      hdgTL->setText(String( _IMU_HDGFiltered, 1) + "°        ");
    if ( _IMU_PitchFiltered < 181 && _IMU_PitchFiltered > -181 )
      pitchTL->setText(String( _IMU_PitchFiltered, 1 ) + "°     ");
    if ( _IMU_HeelFiltered < 181 && _IMU_HeelFiltered > -181 )
      heelTL->setText(String( _IMU_HeelFiltered, 1 ) + "°     ");

  }
}
