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
#include "BarometerN2kConverter.h"
#include <N2kMessages.h>

void BarometerN2kConverter::processMessage(const KMessage &m) {
  m.accept(*this);
}

void BarometerN2kConverter::visit(const BarometerMeasurement &bm) {
  tN2kMsg n2km;
  static int sid = 0;
  // Seems the i70 display will only show sea or outside temperature and
  // outside humidity.
  SetN2kPGN130311(n2km, sid++,
    N2kts_OutsideTemperature, CToKelvin(bm.getTemperature()),
    N2khs_Undef, N2kDoubleNA,
    bm.getPressure()
  );
  sendMessage(NMEA2000Message(n2km));
}
