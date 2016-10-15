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
  NMEASentenceBuilder sb("II", "XDR", 4);
  sb.setField(1, "V");
  sb.setField(2, vm.getVoltage(), 2);
  sb.setField(3, "V");
  sb.setField(4, vm.getLabel());
  nmeaContent += sb.toNMEA() + "\r\n";
}

/* Some helper functions to generate hex-serialized NMEA messages */
static const char *hex = "0123456789ABCDEF";

static int appendByte(char *s, uint8_t byte) {
  s[0] = hex[byte >> 4];
  s[1] = hex[byte & 0xf];
  return 2;
}

static int append2Bytes(char *s, uint16_t i) {
  appendByte(s, i >> 8);
  appendByte(s + 2, i & 0xff);
  return 4;
}

static int appendWord(char *s, uint32_t i) {
  append2Bytes(s, i >> 16);
  append2Bytes(s + 4, i & 0xffff);
  return 8;
}

void KMessageNMEAVisitor::visit(const NMEA2000Message &n2km) {
  // $PCDIN,<PGN 6>,<Timestamp 8>,<src 2>,<data>*20

  const tN2kMsg& msg = n2km.getN2kMsg();

  char *buffer = (char*)malloc(6+1+6+1+8+1+2+1+msg.DataLen*2+1+1+2 + 1);

  char *s = buffer;

  strcpy(buffer, "$PCDIN,");
  buffer += 7;
  buffer += appendByte(buffer, msg.PGN >> 16);
  buffer += append2Bytes(buffer, msg.PGN & 0xffff);
  *buffer++ = ',';
  buffer += appendWord(buffer, n2km.getReceivedMillis());
  *buffer++ = ',';
  buffer += appendByte(buffer, msg.Source);
  *buffer++ = ',';

  for (int i = 0; i < msg.DataLen; i++) {
    buffer += appendByte(buffer, msg.Data[i]);
  }

  *buffer++ = '*';
  buffer += appendByte(buffer, nmea_compute_checksum(s));
  *buffer = 0;

  nmeaContent += String(s) + "\r\n";
  free(s);
}

void KMessageNMEAVisitor::visit(const IMUMessage &imu) {
  NMEASentenceBuilder sb("II", "XDR", 16);
  sb.setField(1, "A");
  sb.setField(2, imu.getHeel(), 1);
  sb.setField(3, "D");
  sb.setField(4, "Heel");
  sb.setField(5, "A");
  sb.setField(6, imu.getPitch(), 1);
  sb.setField(7, "D");
  sb.setField(8, "Pitch");
  sb.setField(9, "A");
  sb.setField(10, imu.getCourse(), 1);
  sb.setField(11, "D");
  sb.setField(12, "Course");
  sb.setField(13, "");
  sb.setField(14, imu.getCalibration());
  sb.setField(15, "");
  sb.setField(16, "Calibration");
  nmeaContent += sb.toNMEA() + "\r\n";
}

