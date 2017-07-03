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

#include "SKVisitor.h"


void SKVisitor::visit(const SKUpdate& u) {
  for (int i = 0; i < u.getSize(); i++) {
    const SKPath &p = u.getPath(i);
    const SKValue &v = u.getValue(i);

    if (p.getStaticPath() == SKPathEnvironmentOutsidePressure) {
      visitSKEnviromentOutsidePressure(u, p, v);
    }
    if (p.getStaticPath() == SKPathElectricalBatteriesVoltage) {
      visitSKElectricalBatteriesVoltage(u, p, v);
    }
    if (p.getStaticPath() == SKPathNavigationAttitude) {
      visitSKNavigationAttitude(u, p, v);
    }
    if (p.getStaticPath() == SKPathNavigationCourseOverGroundTrue) {
      visitSKNavigationCourseOverGround(u, p, v);
    }
    if (p.getStaticPath() == SKPathNavigationHeadingMagnetic) {
      visitSKNavigationHeadingMagnetic(u, p, v);
    }
    if (p.getStaticPath() == SKPathNavigationPosition) {
      visitSKNavigationPosition(u, p, v);
    }
    if (p.getStaticPath() == SKPathNavigationSpeedOverGround) {
      visitSKNavigationSpeedOverGround(u, p, v);
    }
  }
}

