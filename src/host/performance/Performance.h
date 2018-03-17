/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

  Project  :  KBox
  Purpose  :  Performance Calculations on KBox
  Author(s):  Ronnie Zeiller ronnie@zeiller.eu
  *****************************************************************************

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
/*
  Annotation:
  As we are here for sailboat performance calculations we use the common used
  "heel" instead of "roll" as in the rest of KBox code.
*/

#pragma once

#include "common/os/Task.h"
#include "common/signalk/KMessage.h"
#include "host/config/PerformanceConfig.h"
#include "common/signalk/SKHub.h"
#include "common/signalk/SKSubscriber.h"
#include "common/signalk/SKSource.h"

class Performance : public Task, public SKSubscriber {
  private:
    const PerformanceConfig &_config;
    SKHub &_hub;

    double _leeway, _heel, _bs_kts_corr, _awa_m, _aws_m;

    void calcBoatSpeed(double &bs_kts);
    double getCorrForNonLinearTransducer(double &bs_kts, double &heel_deg);
    double getLeeway(double &bs_kts, double &heel);
    void calcApparentWind(double &aws_m, double &awa_m, double &heel);

  public:
    Performance(PerformanceConfig &config, SKHub &skHub);
    ~Performance(){};

    //void setup();
    void loop(){};

    virtual void updateReceived(const SKUpdate& update);

};
