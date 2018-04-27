/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

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


#include <common/time/WallClock.h>
#include "../../KBoxTest.h"
#include "common/signalk/SKContext.h"

static uint32_t mockedTimeMs = 0;
static uint32_t mockTimeProvider() {
  return mockedTimeMs;
}

TEST_CASE("WallClockTest") {
  mockedTimeMs = 42000;
  WallClock clock;

  SECTION("Check default values") {
    CHECK( clock.now().getTime() == 0 );
    CHECK( clock.now().getMilliseconds() == 0 );
    CHECK( !clock.isTimeSet() );
  }

  SECTION("Check you can set and keep the time") {
    SKTime t = SKTime::timeFromNMEAStrings("231282", "193042.450");

    clock.setMillisecondsProvider(mockTimeProvider);
    clock.setTime(t);

    CHECK( clock.now().getTime() == t.getTime() );
    CHECK( clock.now().getMilliseconds() == t.getMilliseconds() );
    CHECK( clock.isTimeSet() );

    SECTION("Time increases with milliseconds") {
      mockedTimeMs += 10;

      CHECK( clock.now().getTime() == t.getTime());
      CHECK( clock.now().getMilliseconds() == t.getMilliseconds() + 10 );

      mockedTimeMs += 10000;

      CHECK( clock.now().getTime() == t.getTime() + 10);
      CHECK( clock.now().getMilliseconds() == t.getMilliseconds() + 10 );
    }

    SECTION("Milliseconds overflow case") {
      mockedTimeMs = UINT32_MAX - 10;

      clock.setTime(t);

      // Can you tell I do not trust myself?

      SECTION("almost overflow") {
        // This will cause an overflow
        mockedTimeMs += 9;

        CHECK( clock.now().getTime() == t.getTime());
        CHECK( clock.now().getMilliseconds() == t.getMilliseconds() + 9);
      }
      SECTION("just overflowed") {
        // This will cause an overflow
        mockedTimeMs += 10;

        CHECK( clock.now().getTime() == t.getTime());
        CHECK( clock.now().getMilliseconds() == t.getMilliseconds() + 10);
      }
      SECTION("just overflowed (bis)") {
        // This will cause an overflow
        mockedTimeMs += 11;

        CHECK( clock.now().getTime() == t.getTime());
        CHECK( clock.now().getMilliseconds() == t.getMilliseconds() + 11);
      }
      SECTION("really overflowed") {
        // This will cause an overflow
        mockedTimeMs += 1000;

        CHECK( clock.now().getTime() == t.getTime() + 1);
        CHECK( clock.now().getMilliseconds() == t.getMilliseconds());
      }
    }
  }

  SECTION("Changing provider should reset time") {
    clock.setTime(SKTime(42, 42));
    CHECK( clock.isTimeSet() );

    clock.setMillisecondsProvider(0);

    CHECK( !clock.isTimeSet() );
  }
}