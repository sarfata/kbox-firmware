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

/*
 * This test executes full conversion of NMEA sentences into SignalK and then
 * back into NMEA2000 messages and validates that the output matches what is
 * expected.
 */

#include "../KBoxTest.h"

TEST_CASE("NMEA-RMC") {
  SECTION("RMC with no satellites") {
  }
  SECTION("Standard RMC") {
    // https://www.fugawi.com/knowledge_base/document/HD25056
    // 126992 system date, system time, time source
    // 127250 heading/deviation/variation
    // 127258 magnetic variation
    // 129025 lat/lon rapid
    // 129026 cog sog rapid
    // 129029 gnss position data
    // 129033 ?

    // Prepare an RMC-like update
    /*
    SKUpdateStatic<3> rmcUpdate;
    SKValue sog = SKValue::navigationSpeedOverGround(3.3);
    SKValue cog = SKValue::navigationCourseOverGroundTrue(42);
    SKValue pos = SKValue::navigationPosition(3.3, 2.2);
    rmcUpdate.addValue(sog);
    rmcUpdate.addValue(cog);
    rmcUpdate.addValue(pos);

    visitor.processUpdate(rmcUpdate);

    //FIXME: Implement missing messages
    //CHECK( visitor.getMessages().size() == 6 );
    CHECK( visitor.getMessages().size() == 2 );

    tN2kMsg *m129026 = findMessage(visitor.getMessages(), 129026);
    CHECK( m129026 != 0 );
    unsigned char sid;
    tN2kHeadingRef ref;
    double cog;
    double sog;
    CHECK( ParseN2kPGN129026(m129026, &sid, &ref, &cog, &sog); );
    CHECK( sid == 0 );
    CHECK( ref == tN2kHeadingRef );
    CHECK( sog == 3.3 );
    CHECK( cog == 42 );

    tN2kMsg *m129025 = findMessage(visitor.getMessages(), 129025);
    CHECK( m129025 != 0 );
    double lat, lon;
    CHECK( ParseN2kPGN129025(m129025,&lat, &lon) );
    CHECK( lat == 3.3 );
    CHECK( lon == 2.2 );
    */

  }
  SECTION("RMC with no heading") {
  }
}
