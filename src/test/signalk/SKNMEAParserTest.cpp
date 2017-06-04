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
#include "common/signalk/SKUnits.h"
#include "common/signalk/SKNMEAParser.h"

TEST_CASE("SKNMEAParserTest: Basic tests") {
  SKNMEAParser p;

  SECTION("Parse invalid sentence") {
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_1, "mambo jumbo");
    CHECK( update.getSize() == 0 );
  }

  SECTION("RMC") {
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_2, "$GPRMC,004119.000,A,3751.3385,N,12227.4913,W,5.02,235.24,141116,,,D*75");

    CHECK( update.getSize() == 3 );
    CHECK( update.getSource() != SKSourceUnknown );
    CHECK( update.getSource().getLabel() == "kbox.nmea0183.2" );
    CHECK( update.getContext() == SKContextSelf );
    CHECK( update.getNavigationSpeedOverGround() == SKKnotToMs(5.02) );
    CHECK( update.getNavigationCourseOverGroundTrue() == SKDegToRad(235.24) );
    CHECK( update.getNavigationPosition().latitude == 37.513385 );
    CHECK( update.getNavigationPosition().longitude == -122.274913 );
    CHECK( update.getNavigationPosition().altitude == 0 );
  }

  SECTION("RMC with invalid fix") {
    const SKUpdate& update = p.parse(SKSourceInputUnknown, "$IIRMC,,V,,,,,,,,009,W,N*2A");

    // This behavior is borrowed from signalk-parser-nmea0183
    // TODO: We should report an alarm here instead of nothing.
    CHECK( update.getSize() == 0 );
  }
}

