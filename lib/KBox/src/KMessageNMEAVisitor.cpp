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

#include "KMessageNMEAVisitor.h"
#include "util/NMEASentenceBuilder.h"

#include <stdio.h>

void KMessageNMEAVisitor::visit(const NMEASentence& s) {
  nmeaContent += s.getSentence() + "\r\n";
}

void KMessageNMEAVisitor::visit(const BarometerMeasurement &bm) {
  // XDR is not a very well defined sentence. Can be used for lots of things
  // apparently but that is better than nothing.
  NMEASentenceBuilder sb("II", "XDR", 8);
  sb.setField(1, "P");
  sb.setField(2, bm.getPressure(), 5);
  sb.setField(3, "B");
  sb.setField(4, "Barometer");
  sb.setField(5, "T");
  sb.setField(6, bm.getTemperature());
  sb.setField(7, "C");
  sb.setField(8, "TempAir");
  nmeaContent += sb.toNMEA() + "\r\n";
}

void KMessageNMEAVisitor::visit(const VoltageMeasurement &vm) {
}

void KMessageNMEAVisitor::visit(const NMEA2000Message &n2km) {
}

void KMessageNMEAVisitor::visit(const IMUMessage &imu) {
}

