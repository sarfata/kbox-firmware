/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

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

#include <N2kMsg.h>
#include <N2kMessages.h>
#include "../KBoxTest.h"
#include "common/signalk/SKUnits.h"

TEST_CASE("SKUnits: Direction Manipulation") {
  // When angle between 0 and 360, it should not change
  CHECK( SKNormalizeDirection(SKDegToRad(90)) == Approx(SKDegToRad(90)) );
  CHECK( SKNormalizeDirection(SKDegToRad(0)) == Approx(SKDegToRad(0)) );
  CHECK( SKNormalizeDirection(SKDegToRad(180)) == Approx(SKDegToRad(180)) );
  CHECK( SKNormalizeDirection(SKDegToRad(181)) == Approx(SKDegToRad(181)) );
  CHECK( SKNormalizeDirection(SKDegToRad(270)) == Approx(SKDegToRad(270)) );

  // Angles above 360 should be converted to values between 0 and 360 (excluded)
  CHECK( SKNormalizeDirection(SKDegToRad(360)) == Approx(SKDegToRad(0)) );
  CHECK( SKNormalizeDirection(SKDegToRad(400)) == Approx(SKDegToRad(40)) );
  CHECK( SKNormalizeDirection(SKDegToRad(30 * 360 + 72)) == Approx(SKDegToRad(72)) );
  CHECK( SKNormalizeDirection(SKDegToRad(30 * 360 - 72)) == Approx(SKDegToRad(360 - 72)) );

  // Angles below 0 should be converted to values between 0 and 360 (excluded)
  CHECK( SKNormalizeDirection(SKDegToRad(-1)) == Approx(SKDegToRad(359)) );
  CHECK( SKNormalizeDirection(SKDegToRad(-360)) == Approx(SKDegToRad(0)) );
  CHECK( SKNormalizeDirection(SKDegToRad(-30 * 360 + 72)) == Approx(SKDegToRad(72)) );
  CHECK( SKNormalizeDirection(SKDegToRad(-30 * 360 - 72)) == Approx(SKDegToRad(360 - 72)) );
}

TEST_CASE("SKUnits: Angle manipulation") {
  // When angle between 0 and 180 (excluded), it should not change
  CHECK( SKNormalizeAngle(SKDegToRad(90)) == Approx(SKDegToRad(90)) );
  CHECK( SKNormalizeAngle(SKDegToRad(0)) == Approx(SKDegToRad(0)) );
  CHECK( SKNormalizeAngle(SKDegToRad(179.999)) == Approx(SKDegToRad(179.999)) );

  // Angles between 180 (included) and 360 should be converted to a value between -180 and +180
  CHECK( SKNormalizeAngle(SKDegToRad(180)) == Approx(SKDegToRad(-180)) );
  CHECK( SKNormalizeAngle(SKDegToRad(181)) == Approx(SKDegToRad(-179)) );
  CHECK( SKNormalizeAngle(SKDegToRad(270)) == Approx(SKDegToRad(-90)) );

  // Angles above 360 should be converted to value between -180 and 180
  CHECK( SKNormalizeAngle(SKDegToRad(360)) == Approx(SKDegToRad(0)) );
  CHECK( SKNormalizeAngle(SKDegToRad(400)) == Approx(SKDegToRad(40)) );
  CHECK( SKNormalizeAngle(SKDegToRad(30 * 360 + 72)) == Approx(SKDegToRad(72)) );
  CHECK( SKNormalizeAngle(SKDegToRad(30 * 360 - 72)) == Approx(SKDegToRad(-72)) );
}
