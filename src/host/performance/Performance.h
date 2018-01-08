/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License
  Copyright (c) 2018 Ronnie Zeiller ronnie@zeiller.eu
  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

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
#include "host/config/PerformanceConfig.h"

class Performance {
  private:
    // const PerformanceConfig &_config;

  public:
    Performance(){};
    bool calcBoatSpeed(double &boatspeed, double &heel, double &leeway);
    bool corrForNonLinearTransducer(double &bs_kts, double &heel);
    bool calcLeeway(double &bs_kts, double &heel, double &leeway);
};
