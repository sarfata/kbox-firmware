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

TEST_CASE("SKNMEAParser: RMC") {
  SKNMEAParser p;

  SECTION("Parse invalid sentence") {
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_1, "mambo jumbo", SKTime(0));
    CHECK( update.getSize() == 0 );
  }

  SECTION("RMC") {
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_2, "$GPRMC,004119.000,A,3751.3385,N,12227.4913,W,5.02,235.24,141116,,,D*75", SKTime(0));

    CHECK( update.getSize() == 3 );
    CHECK( update.getSource() != SKSourceUnknown );
    CHECK( update.getSource().getLabel() == "kbox.nmea0183.2" );
    CHECK( update.getContext() == SKContextSelf );
    CHECK( update.getTimestamp().getTime() == 0 );
    CHECK( update.getNavigationSpeedOverGround() == SKKnotToMs(5.02) );
    CHECK( update.getNavigationCourseOverGroundTrue() == SKDegToRad(235.24) );
    CHECK( update.getNavigationPosition().latitude == 37.85564166666666 );
    CHECK( update.getNavigationPosition().longitude == -122.45818833333334 );
    CHECK( update.getNavigationPosition().altitude == SKDoubleNAN );
  }

  SECTION("RMC with invalid fix") {
    const SKUpdate& update = p.parse(SKSourceInputUnknown, "$IIRMC,,V,,,,,,,,009,W,N*2A", SKTime(0));

    // This behavior is borrowed from signalk-parser-nmea0183
    // TODO: We should report an alarm here instead of nothing.
    CHECK( update.getSize() == 0 );
  }
}

TEST_CASE("SKNMEAParser: MWV") {
  SKNMEAParser p;

  // Ron:
  //$IIMWV,056,R,5.19,N,A*1D
  //$IIMWV,027,T,3.82,N,A*19
  //Johan:
  //$WIMWV,168.1,R,5.6,S,A*33
  //LCJ CV7
  //$IIMWV,227.0,R,000.0,N,A*3A
  SECTION("MWV relative") {
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_1, "$IIMWV,056,R,5.19,N,A*1D", SKTime(42));

    CHECK( update.getSize() == 2 );
    CHECK( update.getSource() != SKSourceUnknown );
    CHECK( update.getContext() == SKContextSelf );
    CHECK( update.getTimestamp().getTime() == 42 );
    CHECK( update.getEnvironmentWindSpeedApparent() == Approx(SKKnotToMs(5.19)) );
    CHECK( update.getEnvironmentWindAngleApparent() == Approx(SKDegToRad(56)) );
  }
  SECTION("MWV true") {
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_1, "$IIMWV,027,T,3.82,N,A*19", SKTime(42));

    CHECK( update.getSize() == 2 );
    CHECK( update.getSource() != SKSourceUnknown );
    CHECK( update.getContext() == SKContextSelf );
    CHECK( update.getTimestamp().getTime() == 42 );
    CHECK( update.getEnvironmentWindSpeedTrue() == SKKnotToMs(3.82) );
    CHECK( update.getEnvironmentWindAngleTrueWater() == SKDegToRad(27) );
  }
  SECTION("MWV relative - unit S") {
    // Unit S seems to be knots but I cannot find a real source here.
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_1, "$WIMWV,168.1,R,5.6,S,A*33", SKTime(42));

    CHECK( update.getEnvironmentWindSpeedApparent() == SKStatuteMphToMs(5.6) );
  }
  SECTION("MWV relative - unit M") {
    // Unit S seems to be knots but I cannot find a real source here.
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_1, "$WIMWV,168.1,R,5.6,M,A*2D", SKTime(42));

    CHECK( update.getEnvironmentWindSpeedApparent() == 5.6 );
  }
  SECTION("MWV relative - unit K") {
    // Unit S seems to be knots but I cannot find a real source here.
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_1, "$WIMWV,168.1,R,5.6,K,A*2B", SKTime(42));

    CHECK( update.getEnvironmentWindSpeedApparent() == SKKmphToMs(5.6) );
  }
  SECTION("MWV with invalid sensor status (V)") {
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_1, "$WIMWV,168.1,R,5.6,S,V*24", SKTime(42));

    CHECK( update.getSize() == 0 );
  }
  SECTION("MWV relative with wind angle > 180") {
    // Unit S seems to be knots but I cannot find a real source here.
    const SKUpdate& update = p.parse(SKSourceInputNMEA0183_1, "$WIMWV,271,R,5.6,K,A*3F", SKTime(42));

    CHECK( update.getEnvironmentWindAngleApparent() == Approx(SKDegToRad(-89)) );
  }
}

TEST_CASE("SKNMEAParser: XDR wind temperature") {
  SKNMEAParser p;

  // From LCJ Wind sensor:
  // $WIXDR,C,030.0,C,,*51
  //
  SECTION("XDR with temperature in C") {
  }
}

