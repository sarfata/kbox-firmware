/*
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

#include "../KBoxTest.h"
#include "common/signalk/SKSource.h"

TEST_CASE("Define a new NMEA0183 source") {
  SKSource nmeaSource = SKSource::sourceForNMEA0183(SKSourceInputNMEA0183_1, "GP", "RMC");

  CHECK( nmeaSource.getType() == "NMEA0183" );
  CHECK( String(nmeaSource.getTalker()) == "GP" );
  CHECK( String(nmeaSource.getSentence()) == "RMC" );
  CHECK( nmeaSource.getLabel() == "kbox.nmea0183.1" );

  // Make sure nmea2000 values are set to reasonable things in case someone would read them.
  CHECK( nmeaSource.getPGN() == 0 );
  CHECK( nmeaSource.getPriority() == 0 );
  CHECK( nmeaSource.getSourceAddress() == 0 );
}

TEST_CASE("An nmea2000 input is invalid for an nmea0183 source") {
  SKSource nmeaSource = SKSource::sourceForNMEA0183(SKSourceInputNMEA2000, "xx", "xxx");

  CHECK( nmeaSource.getType() == "unknown" );
  CHECK( nmeaSource.getLabel() == "kbox.unknown" );
  CHECK( nmeaSource.getTalker() == std::string("") );
  CHECK( nmeaSource.getSentence() == std::string("") );
}

TEST_CASE("An NMEA2000 input") {
  SKSource source = SKSource::sourceForNMEA2000(SKSourceInputNMEA2000, 128042, 1, 5);

  CHECK( source.getPGN() == 128042 );
  CHECK( source.getPriority() == 1 );
  CHECK( source.getSourceAddress() == 5 );
}
