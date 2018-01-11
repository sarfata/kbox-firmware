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

#include "common/algo/List.h"
#include "common/signalk/SKUnits.h"
#include "common/signalk/SKNMEAConverter.h"
#include "common/signalk/SKUpdateStatic.h"
#include "../KBoxTest.h"

class NMEAOut : public LinkedList<SKNMEASentence>, public SKNMEAOutput {
  public:
    bool write(const SKNMEASentence& s) override {
      add(s);
      return true;
    };

};

TEST_CASE("SKNMEAConverterTest") {
  SKNMEAConverterConfig config;
  SKNMEAConverter converter(config);
  NMEAOut out;

  SECTION("ElectricalBatteriesVoltage") {
    SKUpdateStatic<3> u;
    u.setElectricalBatteriesVoltage("Supply", 12.42);

    converter.convert(u, out);

    CHECK( out.size() == 1 );

    if (out.size() > 0) {
      String s = *(out.begin());
      CHECK( s == "$IIXDR,V,12.42,V,Supply*56");
    }
  }

  SECTION("EnvironmentOutsidePressure") {
    SKUpdateStatic<1> u;
    u.setEnvironmentOutsidePressure(102421);

    converter.convert(u, out);

    CHECK( out.size() == 1 );

    if (out.size() > 0) {
      String s = *(out.begin());
      CHECK( s == "$IIXDR,P,1.02421,B,Barometer*23" );
    }
  }

  SECTION("NavigationAttitude") {
    SKUpdateStatic<1> u;
    u.setNavigationAttitude(SKTypeAttitude(/*roll*/SKDegToRad(29),
          /*pitch*/SKDegToRad(10.1),
          /*yaw*/SKDegToRad(4.2)));

    converter.convert(u, out);

    CHECK( out.size() == 1 );

    if (out.size() > 0) {
      String s = *(out.begin());
      CHECK( s == "$IIXDR,A,10.1,D,PTCH,A,29.0,D,ROLL*57" );
    }
  }

  SECTION("NavigationAttitude with NAN for pitch") {
    SKUpdateStatic<1> u;
    u.setNavigationAttitude(SKTypeAttitude(/*roll*/SKDegToRad(29),
          /*pitch*/SKDoubleNAN,
          /*yaw*/SKDoubleNAN));

    converter.convert(u, out);

    CHECK( out.size() == 1 );

    if (out.size() > 0) {
      String s = *(out.begin());
      CHECK( s == "$IIXDR,A,,D,PTCH,A,29.0,D,ROLL*49" );
    }
  }

  SECTION("NavigationHeadingMagnetic") {
    SKUpdateStatic<1> u;
    u.setNavigationHeadingMagnetic(SKDegToRad(142));

    SECTION("Basic conversion") {
      converter.convert(u, out);

      CHECK( out.size() == 1 );

      if (out.size() > 0) {
        String s = *(out.begin());
        CHECK( s == "$IIHDM,142.0,M*25" );
      }
    }
    SECTION("Disable HDM") {
      config.hdm = false;

      converter.convert(u, out);
      CHECK( out.size() == 0 );
    }
  }

  SECTION("MWV") {
    SKUpdateStatic<2> u;

    SECTION("True wind") {
      u.setEnvironmentWindSpeedTrue(10);
      u.setEnvironmentWindAngleTrueWater(SKDegToRad(142));

      converter.convert(u, out);
      CHECK( out.size() == 1 );
      if (out.size() > 0) {
        String s = *(out.begin());
        CHECK( s == "$IIMWV,142.0,T,10.00,M,A*3E" );
      }
    }

    SECTION("Apparent wind") {
      u.setEnvironmentWindSpeedApparent(10);
      u.setEnvironmentWindAngleApparent(SKDegToRad(142));

      converter.convert(u, out);
      CHECK( out.size() == 1 );
      if (out.size() > 0) {
        String s = *(out.begin());
        CHECK( s == "$IIMWV,142.0,R,10.00,M,A*38" );
      }
    }

    SECTION("Negative angle") {
      u.setEnvironmentWindSpeedApparent(10);
      u.setEnvironmentWindAngleApparent(SKDegToRad(-30));

      converter.convert(u, out);
      CHECK( out.size() == 1 );
      if (out.size() > 0) {
        String s = *(out.begin());
        CHECK( s == "$IIMWV,330.0,R,10.00,M,A*3F" );
      }
    }

    SECTION("Incomplete data") {
      u.setEnvironmentWindSpeedApparent(10);
      converter.convert(u, out);
      CHECK( out.size() == 0 );
    }

    SECTION("Incomplete data") {
      u.setEnvironmentWindSpeedApparent(10);
      u.setEnvironmentWindSpeedTrue(10);
      converter.convert(u, out);
      CHECK( out.size() == 0 );
    }
  }

  SECTION("RSA") {
    SKUpdateStatic<1> u;

    SECTION("Basic test") {
      u.setSteeringRudderAngle(SKDegToRad(10));
      converter.convert(u, out);
      CHECK( out.size() == 1 );
      if (out.size() > 0) {
        String s = *(out.begin());
        CHECK( s == "$IIRSA,10.0,A,,*1E" );
      }
    }

    SECTION("Negative angle") {
      u.setSteeringRudderAngle(SKDegToRad(-10));
      converter.convert(u, out);
      CHECK( out.size() == 1 );
      if (out.size() > 0) {
        String s = *(out.begin());
        CHECK( s == "$IIRSA,-10.0,A,,*33" );
      }
    }
  }
}
