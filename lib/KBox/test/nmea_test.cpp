/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#define CATCH_CONFIG_MAIN
#include "catch.hpp"


#include <stdarg.h>
#define DEBUG(...) debug(__FILE__, __LINE__, __VA_ARGS__)

void debug(const char *fname, int lineno, const char *fmt, ... ) {
  printf("%s: %i ", fname, lineno);
  char tmp[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(tmp, 128, fmt, args);
  va_end (args);
  printf("%s\n", tmp);
}


#include "util/nmea.h"

TEST_CASE("nmea basic") {
  WHEN("sentence is empty") {
    REQUIRE( nmea_is_valid("") == false);
    REQUIRE( nmea_is_valid(" ") == false);
    REQUIRE( nmea_is_valid("\r\n") == false);
  }

  WHEN("sentence is a valid GGA sentence") {
    REQUIRE( nmea_is_valid("$GPGGA,003516.000,3751.6035,N,12228.8065,W,2,10,0.91,3.4,M,-25.2,M,0000,0000*5A") );
    REQUIRE( nmea_is_valid("$GPGGA,003516.000,3751.6035,N,12228.8065,W,2,10,0.91,3.4,M,-25.2,M,0000,0000*5A\r\n") );
    REQUIRE( nmea_is_valid("$GPGGA,003516.000,3751.6035,N,12228.8065,W,2,10,0.91,3.4,M,-25.2,M,0000,0000*") );
    REQUIRE( nmea_is_valid("$GPGGA,003516.000,3751.6035,N,12228.8065,W,2,10,0.91,3.4,M,-25.2,M,0000,0000*\r\n") );
  }

  WHEN("sentence is a valid AIS sentence") {
    REQUIRE( nmea_is_valid("!AIVDM,1,1,,B,ENkb9I9I7@@@@@@@@@@@@@@@@@@;V4=v:nv;h00003vP000,2*54") );
    REQUIRE( nmea_is_valid("!AIVDM,1,1,,B,ENkb9I9I7@@@@@@@@@@@@@@@@@@;V4=v:nv;h00003vP000,2*") );
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

}
