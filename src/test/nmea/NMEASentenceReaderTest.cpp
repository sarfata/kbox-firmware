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

#include "common/nmea/NMEASentenceReader.h"
#include "../KBoxTest.h"
#include <cmath>

// We have a conflict here between KBox which uses math.h and Catch which uses cmath
#define isnan(x) std::isnan(x)

TEST_CASE("NMEASentenceReader") {

  SECTION("RMC Sentence") {
    NMEASentenceReader r("$GPRMC,144629.20,A,5156.91111,N,00434.80385,E,0.295,,011113,,,A*78");

    CHECK( r.isValid() );
    CHECK( r.getTalkerId() == "GP" );
    CHECK( r.getSentenceCode() == "RMC" );
    CHECK( r.countFields() == 12 );
    CHECK( r.getFieldAsString(1) == "144629.20" );
    CHECK( r.getFieldAsChar(2) == 'A' );
    CHECK( r.getFieldAsDouble(3) == 5156.91111 );
    CHECK( r.getFieldAsChar(4) == 'N' );
    CHECK( r.getFieldAsDouble(5) == 434.80385 );
    CHECK( r.getFieldAsChar(6) == 'E' );
    CHECK( r.getFieldAsDouble(7) == 0.295);
    CHECK( isnan(r.getFieldAsDouble(8)) );
    CHECK( r.getFieldAsString(9) == "011113");
    CHECK( isnan(r.getFieldAsDouble(10)) );
    CHECK( r.getFieldAsChar(11) == '\0' );
    CHECK( r.getFieldAsChar(12) == 'A' );


    SECTION("Accessing fields that do not exist") {
      CHECK( r.getFieldAsString(42) == "" );
      CHECK( r.getFieldAsChar(42) == '\0' );
      CHECK( isnan(r.getFieldAsDouble(42)) );
    }
  }

  SECTION("Invalid checksum") {
    NMEASentenceReader r("$GPRMC,144629.20,A,5156.91111,N,00434.80385,E,0.295,,011113,,,B*78");

    CHECK( ! r.isValid() );
  }

  SECTION("mambo jumbo") {
    NMEASentenceReader r("mambo jumbo!");

    CHECK( ! r.isValid() );
  }

  SECTION("empty string") {
    NMEASentenceReader r("");

    CHECK( ! r.isValid() );
  }

  SECTION("No checksum") {
    NMEASentenceReader r("$GPRMC,144629.20,A,5156.91111,N,00434.80385,E,0.295,,011113,,,B*");

    CHECK( ! r.isValid() );
  }

  SECTION("AIS sentence") {
    NMEASentenceReader r("!AIVDM,1,1,,A,ENkb9KqJch@@@@@@@@@@@@@@@@@;VDE>:gnMP00003vP000,2*7D");

    CHECK( r.isValid() );
    CHECK( r.getTalkerId() == "AI" );
    CHECK( r.getSentenceCode() == "VDM" );
    CHECK( r.countFields() == 6 );
  }

  SECTION("Reading angles") {
    NMEASentenceReader r("$TST,2832.1834,N,08101.0536,W*");

    CHECK( r.getFieldAsLatLon(1) == 28.536390 );
    CHECK( r.getFieldAsLatLon(3) == -81.017560 );
  }

  SECTION("Reading angles close to 0") {
    NMEASentenceReader r("$TST,002.1834,N,001.0536,W*");

    CHECK( r.getFieldAsLatLon(1) == 0.036390 );
    CHECK( r.getFieldAsLatLon(3) == Approx(-0.01756) );
  }

  SECTION("Reading angles close to 0 - and removing extra 0s") {
    // I am not sure if that is actually valid...
    NMEASentenceReader r("$TST,2.1834,N,1.0536,W*");

    CHECK( r.getFieldAsLatLon(1) == 0.036390 );
    CHECK( r.getFieldAsLatLon(3) == Approx(-0.01756) );
  }

  SECTION("Reading angles - with invalid sign") {
    // I am not sure if that is actually valid...
    NMEASentenceReader r("$TST,2.1834,1.0536,G,42*");

    CHECK( isnan(r.getFieldAsLatLon(1)) );
    CHECK( isnan(r.getFieldAsLatLon(2)) );
    CHECK( isnan(r.getFieldAsLatLon(4)) );

  }
}

