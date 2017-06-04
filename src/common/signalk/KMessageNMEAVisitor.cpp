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
#include "nmea/NMEASentenceBuilder.h"
#include "nmea/nmea2000.h"

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
  sb.setField(6, bm.getTemperature(), 2);
  sb.setField(7, "C");
  sb.setField(8, "TempAir");
  nmeaContent += sb.toNMEA() + "\r\n";
}

void KMessageNMEAVisitor::visit(const NMEA2000Message &n2km) {
  // $PCDIN,<PGN 6>,<Timestamp 8>,<src 2>,<data>*20

  const tN2kMsg& msg = n2km.getN2kMsg();
  char *s = nmea_pcdin_sentence_for_n2kmsg(msg, n2km.getReceivedMillis() / 1000);
  nmeaContent += String(s) + "\r\n";
  free(s);
}

void KMessageNMEAVisitor::visit(const IMUMessage &imu) {
  NMEASentenceBuilder sb("II", "XDR", 16);
  sb.setField(1, "A");
  sb.setField(2, imu.getYaw(), 1);
  sb.setField(3, "D");
  sb.setField(4, "Yaw");

  sb.setField(5, "A");
  sb.setField(6, imu.getPitch(), 1);
  sb.setField(7, "D");
  sb.setField(8, "Pitch");

  sb.setField(9, "A");
  sb.setField(10, imu.getRoll(), 1);
  sb.setField(11, "D");
  sb.setField(12, "Roll");

  sb.setField(13, "");
  sb.setField(14, String(imu.getCalibration()));
  sb.setField(15, "");
  sb.setField(16, "Calibration");

  nmeaContent += sb.toNMEA() + "\r\n";

  NMEASentenceBuilder sb2("II", "HDM", 2);
  sb2.setField(1, imu.getCourse(), 2);
  sb2.setField(2, "M");

  nmeaContent += sb2.toNMEA() + "\r\n";
}

