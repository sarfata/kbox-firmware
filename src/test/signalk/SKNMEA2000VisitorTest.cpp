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

#include <stdint.h>
#include <N2kMsg.h>
#include <N2kMessages.h>
#include "../KBoxTest.h"
#include "common/signalk/SKNMEA2000Visitor.h"
#include "common/signalk/SKUpdateStatic.h"
#include "util.h"

TEST_CASE("NMEA2000Visitor") {
  SKNMEA2000Visitor visitor;

  SECTION("Basic checks") {
    CHECK( visitor.getMessages().size() == 0 );
  }

  SECTION("SOG/COG") {
    SKUpdateStatic<2> rmcUpdate;
    rmcUpdate.setValue(SKPathNavigationSpeedOverGround, 3);
    rmcUpdate.setValue(SKPathNavigationCourseOverGroundTrue, 1);

    visitor.processUpdate(rmcUpdate);

    CHECK( visitor.getMessages().size() == 1 );

    const tN2kMsg *m129026 = findMessage(visitor.getMessages(), 129026, 0);
    CHECK( m129026 != 0 );
    if (m129026) {
      unsigned char sid;
      tN2kHeadingReference ref;
      double cog;
      double sog;

      CHECK( ParseN2kPGN129026(*m129026, sid, ref, cog, sog) );
      CHECK( sid == 0 );
      CHECK( ref == N2khr_true );
      CHECK( sog == 3 );
      CHECK( cog == 1 );
    }
  }

}

