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

#include "common/signalk/SKUnits.h"
#include "common/signalk/SKNMEAVisitor.h"
#include "common/signalk/SKUpdateStatic.h"
#include "../KBoxTest.h"

TEST_CASE("SKNMEAVisitorTest") {
  SKNMEAVisitor v;

  SECTION("ElectricalBatteriesVoltage") {
    SKUpdateStatic<3> u;
    u.setElectricalBatteriesVoltage("Supply", 12.42);

    v.processUpdate(u);

    CHECK( v.getSentences().size() == 1 );

    if (v.getSentences().size() > 0) {
      String s = *(v.getSentences().begin());
      CHECK( s == "$IIXDR,V,12.42,V,Supply*56\r\n");
    }
  }

  SECTION("EnvironmentOutsidePressure") {
    SKUpdateStatic<1> u;
    u.setEnvironmentOutsidePressure(1.02421);

    v.processUpdate(u);

    CHECK( v.getSentences().size() == 1 );

    if (v.getSentences().size() > 0) {
      String s = *(v.getSentences().begin());
      CHECK( s == "$IIXDR,P,1.02421,B,Barometer*23\r\n" );
    }
  }

  SECTION("NavigationHeadingMagnetic") {
    SKUpdateStatic<1> u;
    u.setNavigationHeadingMagnetic(SKDegToRad(142));

    v.processUpdate(u);

    CHECK( v.getSentences().size() == 1 );

    if (v.getSentences().size() > 0) {
      String s = *(v.getSentences().begin());
      CHECK( s == "$IIHDM,142.0,M*25\r\n" );
    }
  }

  SECTION("NavigationAttitude") {
    SKUpdateStatic<1> u;
    u.setNavigationAttitude(SKTypeAttitude(/*roll*/SKDegToRad(29),
          /*pitch*/SKDegToRad(10.1),
          /*yaw*/SKDegToRad(4.2)));

    v.processUpdate(u);

    CHECK( v.getSentences().size() == 1 );

    if (v.getSentences().size() > 0) {
      String s = *(v.getSentences().begin());
      CHECK( s == "$IIXDR,A,10.1,D,PTCH,A,29.0,D,ROLL*57\r\n" );
    }
  }

}
