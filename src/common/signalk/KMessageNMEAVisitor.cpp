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

void KMessageNMEAVisitor::visit(const NMEA2000Message &n2km) {
  // $PCDIN,<PGN 6>,<Timestamp 8>,<src 2>,<data>*20

  const tN2kMsg& msg = n2km.getN2kMsg();
  char *s = nmea_pcdin_sentence_for_n2kmsg(msg, n2km.getReceivedMillis() / 1000);
  nmeaContent += String(s) + "\r\n";
  free(s);
}

