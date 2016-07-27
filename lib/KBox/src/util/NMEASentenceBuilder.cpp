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

#include "NMEASentenceBuilder.h"
#include "nmea.h"

static const char *hex = "0123456789ABCDEF";

NMEASentenceBuilder::NMEASentenceBuilder(String talkerId, String sentenceId, int numFields) :
  talkerId(talkerId), sentenceId(sentenceId), numFields(numFields)
{
  fields = new String[numFields];
}

NMEASentenceBuilder::~NMEASentenceBuilder() {
  delete[] fields;
}

void NMEASentenceBuilder::setField(int fieldId, String v) {
  if (fieldId > 0 && fieldId <= numFields) {
    fields[fieldId - 1] = v;
  }
}

void NMEASentenceBuilder::setField(int fieldId, float v, int precision) {
  setField(fieldId, String(v, precision));
}

String NMEASentenceBuilder::toNMEA() {
  String nmea = "$" + talkerId + sentenceId;

  for (int i = 0; i < numFields; i++) {
    nmea += "," + fields[i];
  }

  nmea += "*";

  uint8_t checksum = nmea_compute_checksum(nmea.c_str());
  nmea += hex[checksum / 16];
  nmea += hex[checksum % 16];

  return nmea;
}
