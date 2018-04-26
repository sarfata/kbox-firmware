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

#include "common/signalk/SKTime.h"
#include "../KBoxTest.h"

TEST_CASE("SKTime") {
  SECTION("Creation without ms") {
    SKTime t(409516200);
    CHECK( t.getTime() == 409516200 );
    CHECK( !t.hasMilliseconds() );
    CHECK( t.getMilliseconds() == 0 );
    CHECK( t.toString() == "1982-12-23T18:30:00Z" );
  }

  SECTION("Creation with ms") {
    SKTime t(409516200, 42);
    CHECK( t.getTime() == 409516200 );
    CHECK( t.hasMilliseconds() );
    CHECK( t.getMilliseconds() == 42 );
    CHECK( t.toString() == "1982-12-23T18:30:00.042Z" );
  }

  SECTION("less than 10 day, month, hour, minute, second") {
    SKTime t(946706828);
    CHECK( t.toString() == "2000-01-01T06:07:08Z" );
  }

  SECTION("ms edge cases") {
    WHEN("0") {
      SKTime t(409516200, 0);
      CHECK( t.toString() == "1982-12-23T18:30:00.000Z" );
    }
    WHEN("1") {
      SKTime t(409516200, 1);
      CHECK( t.toString() == "1982-12-23T18:30:00.001Z" );
    }
    WHEN("10") {
      SKTime t(409516200, 10);
      CHECK( t.toString() == "1982-12-23T18:30:00.010Z" );
    }
    WHEN("42") {
      SKTime t(409516200, 42);
      CHECK( t.toString() == "1982-12-23T18:30:00.042Z" );
    }
    WHEN("100") {
      SKTime t(409516200, 100);
      CHECK( t.toString() == "1982-12-23T18:30:00.100Z" );
    }
    WHEN("999") {
      SKTime t(409516200, 999);
      CHECK( t.toString() == "1982-12-23T18:30:00.999Z" );
    }
  }

  SECTION("Invalid ms") {
    SKTime t(409516200, 1001);
    CHECK( !t.hasMilliseconds() );
    CHECK( t.getMilliseconds() == 0 );
    CHECK( t.toString() == "1982-12-23T18:30:00Z" );
  }

  SECTION("Parse NMEA date/time without ms") {
    SKTime t("141116", "004119");
    CHECK( t.toString() == "2016-11-14T00:41:19Z" );
  }

  SECTION("Parse NMEA date/time with 000ms") {
    SKTime t("141116", "004119.000");
    CHECK( t.toString() == "2016-11-14T00:41:19.000Z" );
  }

  SECTION("Parse NMEA date/time with ms") {
    SKTime t("141116", "004119.042");
    CHECK( t.toString() == "2016-11-14T00:41:19.042Z" );
  }

  SECTION("Invalid milliseconds") {
    SKTime t("141116", "004119.10212");
    CHECK( t.toString() == "2016-11-14T00:41:19.102Z" );
  }

  SECTION("2 digits milliseconds") {
    SKTime t("141116", "004119.10");
    CHECK( t.toString() == "2016-11-14T00:41:19.100Z" );
  }

  SECTION("1 digits milliseconds") {
    SKTime t("141116", "004119.9");
    CHECK( t.toString() == "2016-11-14T00:41:19.900Z" );
  }

  SECTION("Programmed obsolescence") {
    SECTION("Time before y2000") {
      SKTime t("231282", "193000");
      CHECK( t.toString() == "1982-12-23T19:30:00Z" );
    }

    SECTION("Jan 1 1970") {
      SKTime t("010170", "010203");
      CHECK( t.getTime() == 1 * 3600 + 2 * 60 + 3 );
      CHECK( t.toString() == "1970-01-01T01:02:03Z" );
    }
  }
}

