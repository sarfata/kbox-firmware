/*
  * Project:  KBox
  * Purpose:  IMU Monitor Page for HDG, Cal, Heel & Pitch
  * Author:   (c) Ronnie Zeiller, 2017
  * KBox:     (c) 2016 Thomas Sarlandie thomas@sarlandie.net

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

#include "common/ui/Page.h"
#include "common/ui/TextLayer.h"
#include "common/signalk/SKHub.h"
#include "common/signalk/SKSubscriber.h"
#include "util/iirfilter.h"

class IMUMonitorPage : public Page, public SKSubscriber {
private:
  TextLayer *hdgTL, *heelTL, *pitchTL, *calTL;  // display cal planned for later
  unsigned long int _nextPrint = 0;
  uint16_t _IMUServiceInterval = 50;

  iirfilter   IMU_HdgFilter;
  iirfilter   IMU_HeelFilter;
  iirfilter   IMU_PitchFilter;

  double _dampingHdg;
  double _dampingHeelPitch;

  double _IMU_HDGFiltered;
  double _IMU_PitchFiltered;
  double _IMU_HeelFiltered;


public:
  IMUMonitorPage(SKHub &hub);


  virtual void updateReceived(const SKUpdate &up);
};
