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
  SKValue cog = SKValue::navigationCourseOverGroundTrue(1.323);
  CHECK( cog.getPath() == SKPathNavigationCourseOverGroundTrue );
  CHECK( cog.getNavigationCourseOverGroundTrue() == 1.323);

  SKValue sog = SKValue::navigationSpeedOverGround(21.22);
  CHECK( sog.getPath() == SKPathNavigationSpeedOverGround );
  CHECK( sog.getNavigationSpeedOverGround() == 21.22);

  SKValue pos = SKValue::navigationPosition(32.33223, 32.4343);
  CHECK( pos.getPath() == SKPathNavigationPosition );
  CHECK( pos.getNavigationPositionLatitude() == 32.33223);
  CHECK( pos.getNavigationPositionLongitude() == 32.4343);

  CHECK( SKValueNone.getPath() == SKPathInvalid );
  CHECK( SKValueNone == SKValue::noneValue() );
};


