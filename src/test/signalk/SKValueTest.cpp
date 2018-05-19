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
#include "common/signalk/SKValue.h"

TEST_CASE("SKValue: Create a few values and make sure we can read their data back") {
  SECTION("Double Value") {
    SKValue cog = SKValue(1.323);
    CHECK( cog.getNumberValue() == 1.323 );

    // Check that reading the wrong type will always return 0
    CHECK( cog.getPositionValue().latitude == 0 );
    CHECK( cog.getPositionValue().longitude == 0 );
    CHECK( cog.getPositionValue().altitude == 0 );
  }


  SECTION("Position Value") {
    SKValue pos = SKValue(SKTypePosition(34.34, -178, 2));
    CHECK( pos.getPositionValue().latitude == 34.34 );
    CHECK( pos.getPositionValue().longitude == -178 );
    CHECK( pos.getPositionValue().altitude == 2 );

    // Check that reading the wrong type will always return 0
    CHECK( pos.getNumberValue() == 0 );
  }

  SECTION("Attitude value") {
    SKValue attitude = SKValue(SKTypeAttitude(33.0, 10.1, 0));

    CHECK( attitude.getAttitudeValue().roll == 33.0 );
    CHECK( attitude.getAttitudeValue().pitch == 10.1 );
    CHECK( attitude.getAttitudeValue().yaw == 0 );

    CHECK( attitude.getNumberValue() == 0 );
  }

  SECTION("Timestamp value") {
    SKValue ts = SKValue(SKTime(1524764848, 102));

    CHECK( ts.getTimestampValue().getTime() == 1524764848 );
    CHECK( ts.getTimestampValue().getMilliseconds() == 102 );
    CHECK( ts.getTimestampValue().toString() == "2018-04-26T17:47:28.102Z");
  }
};


