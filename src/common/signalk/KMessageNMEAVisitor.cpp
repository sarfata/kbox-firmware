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

#include <KBoxLogging.h>
#include <Seasmart.h>
#include "KMessageNMEAVisitor.h"
#include "nmea/NMEASentenceBuilder.h"

void KMessageNMEAVisitor::visit(const NMEASentence& s) {
  if ( _config.dataFormat == NMEA_Seasmart || _config.dataFormat == NMEA) {
    DEBUG("Sentence: %s", s.getSentence());
    nmeaContent += s.getSentence() + "\r\n";
  }
}

void KMessageNMEAVisitor::visit(const NMEA2000Message &n2km) {

  const tN2kMsg& msg = n2km.getN2kMsg();
  
  DEBUG("ConfigDataFormat: %i",_config.dataFormat);
  if ( _config.dataFormat == NMEA_Seasmart || _config.dataFormat == Seasmart) {
    if (msg.DataLen < 500) {
      char pcdin[30 + msg.DataLen * 2];
      N2kToSeasmart(msg, n2km.getReceivedTime(), pcdin, sizeof(pcdin));
      // $PCDIN,<PGN 6>,<Timestamp 8>,<src 2>,<data>*20
      DEBUG(pcdin);
      nmeaContent += String(pcdin) + "\r\n";
    }
  }
}
