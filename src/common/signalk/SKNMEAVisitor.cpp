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

#include "SKUnits.h"
#include "common/nmea/NMEASentenceBuilder.h"

#include "SKNMEAVisitor.h"

void SKNMEAVisitor::visitSKElectricalBatteriesVoltage(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  NMEASentenceBuilder sb("II", "XDR", 4);
  sb.setField(1, "V");
  sb.setField(2, v.getNumberValue(), 2);
  sb.setField(3, "V");
  sb.setField(4, p.getIndex());
  _sentences.add(sb.toNMEA() + "\r\n");
}

void SKNMEAVisitor::visitSKEnvironmentOutsidePressure(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  // XDR is not a very well defined sentence. Can be used for lots of things
  // apparently but that is better than nothing.
  NMEASentenceBuilder sb("II", "XDR", 4);
  sb.setField(1, "P");
  sb.setField(2, v.getNumberValue(), 5);
  sb.setField(3, "B");
  sb.setField(4, "Barometer");
  _sentences.add(sb.toNMEA() + "\r\n");
}

void SKNMEAVisitor::visitSKNavigationAttitude(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  // Same comment on XDR. It's not very well standardized but at least its a way
  // to share the data.
  NMEASentenceBuilder sb( "II", "XDR", 8);
  sb.setField(1, "A");
  sb.setField(2, SKRadToDeg(v.getAttitudeValue().pitch), 1);
  sb.setField(3, "D");
  sb.setField(4, "PTCH");

  sb.setField(5, "A");
  sb.setField(6, SKRadToDeg(v.getAttitudeValue().roll), 1);
  sb.setField(7, "D");
  sb.setField(8, "ROLL");

  _sentences.add(sb.toNMEA() + "\r\n");
};

void SKNMEAVisitor::visitSKNavigationHeadingMagnetic(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  NMEASentenceBuilder sb2("II", "HDM", 2);
  sb2.setField(1, SKRadToDeg(v.getNumberValue()), 1);
  sb2.setField(2, "M");

  _sentences.add(sb2.toNMEA() + "\r\n");
};
