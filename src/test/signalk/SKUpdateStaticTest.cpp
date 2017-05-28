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
#include "common/signalk/SKUpdateStatic.h"

TEST_CASE("SKUpdateStatic") {
  SKUpdateStatic<2> u;
  CHECK( u.getSize() == 0 );
  CHECK( u[42] == SKValueNone );
  CHECK( u[SKPathNavigationPosition] == SKValueNone );

  CHECK( u.getSource() == SKSourceUnknown );
  CHECK( u.getContext() == SKContextSelf );

  SECTION("Add one value") {
    SKValue sog = SKValue::navigationSpeedOverGround(3.3);

    CHECK( u.addValue(sog) );

    CHECK( u.getSize() == 1 );
    CHECK( u[0] != SKValueNone );
    CHECK( u[SKPathNavigationPosition] == SKValueNone );
    CHECK( u[SKPathNavigationSpeedOverGround].getNavigationSpeedOverGround() == sog.getNavigationSpeedOverGround() );
  }

  SECTION("Add too many values") {
    SKValue sog = SKValue::navigationSpeedOverGround(3.3);
    SKValue cog = SKValue::navigationCourseOverGroundTrue(42);
    SKValue pos = SKValue::navigationPosition(3.3, 2.2);

    CHECK( u.addValue(sog) );
    CHECK( u.addValue(cog) );
    CHECK( ! u.addValue(pos) );

    CHECK( u.getSize() == 2 );
    CHECK( u[SKPathNavigationSpeedOverGround].getNavigationSpeedOverGround() == 3.3 );
    CHECK( u[SKPathNavigationPosition] == SKValueNone );
  }

  SECTION("Add a source") {
    SKSource s = SKSource::sourceForNMEA0183(SKSourceInputNMEA0183_1, "GP", "RMC");
    u.setSource(s);

    CHECK( u.getSource() == s );
  }
};

