/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

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
#include "common/nmea/nmea.h"

TEST_CASE("nmea basic") {
  WHEN("sentence is empty") {
    REQUIRE( nmea_is_valid("") == false);
    REQUIRE( nmea_is_valid(" ") == false);
    REQUIRE( nmea_is_valid("\r\n") == false);
  }

  WHEN("sentence is a valid GGA sentence") {
    REQUIRE( nmea_is_valid("$GPGGA,003516.000,3751.6035,N,12228.8065,W,2,10,0.91,3.4,M,-25.2,M,0000,0000*5A") );
    REQUIRE( nmea_is_valid("$GPGGA,003516.000,3751.6035,N,12228.8065,W,2,10,0.91,3.4,M,-25.2,M,0000,0000*5A\r\n") );
  }

  WHEN("sentence is a valid AIS sentence") {
    REQUIRE( nmea_is_valid("!AIVDM,1,1,,B,ENkb9I9I7@@@@@@@@@@@@@@@@@@;V4=v:nv;h00003vP000,2*54") );
  }

  WHEN("sentence is invalid (hacked off one byte)") {
    REQUIRE( !nmea_is_valid("$GPGGA,00351.000,3751.6035,N,12228.8065,W,2,10,0.91,3.4,M,-25.2,M,0000,0000*5A") );
    REQUIRE( !nmea_is_valid("!AIVDM,1,1,,B,Nkb9I9I7@@@@@@@@@@@@@@@@@@;V4=v:nv;h00003vP000,2*54") );
  }

  WHEN("sentence is invalid (does not start with $ or !)") {
    REQUIRE( !nmea_is_valid("GPGGA,xxxx,*") );
    REQUIRE( !nmea_is_valid("GPGGA,003516.000,3751.6035,N,12228.8065,W,2,10,0.91,3.4,M,-25.2,M,0000,0000*5A") );
  }

  WHEN("sentence is invalid (invalid checksum)") {
    REQUIRE( !nmea_is_valid("$GPGGA,003516.000,3751.6035,N,12228.8065,W,2,10,0.91,3.4,M,-25.2,M,0000,0000*5B") );
  }

  WHEN("sentence is invalid (garbled)") {
    REQUIRE( !nmea_is_valid("$IIMTW,8") );
  }
  WHEN("sentence is invalid (garbled - invalid chars)") {
    REQUIRE( !nmea_is_valid("$IIRMB,V,,%çÿÝÕ_£ÿíÿuóÜ;»Õ·_ÖÿW5ÿÙ") );
  }
}
