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
#include "common/signalk/SKUpdateStatic.h"
#include "common/signalk/SKNMEA2000Visitor.h"
#include "common/signalk/SKUnits.h"
#include "util.h"

#define ApproxN2kWindSpeed(x) Approx(x).epsilon(0.01)
#define ApproxN2kWindAngle(x) Approx(x).epsilon(0.0001)

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

  SECTION("Electrical") {
    SKUpdateStatic<2> electricalUpdate;
    electricalUpdate.setElectricalBatteriesVoltage("engine", 13.0);
    electricalUpdate.setElectricalBatteriesVoltage("house", 12.0);

    visitor.processUpdate(electricalUpdate);

    CHECK( visitor.getMessages().size() == 2 );

    if (visitor.getMessages().size() > 2) {
      const tN2kMsg *m1 = findMessage(visitor.getMessages(), 127508, 0);
      const tN2kMsg *m2 = findMessage(visitor.getMessages(), 127508, 1);

      unsigned char instance;
      double voltage;
      double current;
      double temperature;
      unsigned char sid1, sid2;

      CHECK( ParseN2kPGN127508(*m1, instance, voltage, current, temperature, sid1) );
      CHECK( instance == 0 );
      CHECK( voltage == 13.0 );
      CHECK( current == N2kDoubleNA );
      CHECK( temperature == N2kDoubleNA );

      CHECK( ParseN2kPGN127508(*m2, instance, voltage, current, temperature, sid2) );
      CHECK( instance == 1 );
      CHECK( voltage == 12.0 );
      CHECK( current == N2kDoubleNA );
      CHECK( temperature == N2kDoubleNA );
      CHECK( sid1 == sid2 );
    }
  }

  SECTION("Barometer") {
    SKUpdateStatic<1> pressureUpdate;
    pressureUpdate.setEnvironmentOutsidePressure(1.013);

    visitor.processUpdate(pressureUpdate);

    CHECK( visitor.getMessages().size() == 1 );

    if (visitor.getMessages().size() > 1) {
      const tN2kMsg *m = findMessage(visitor.getMessages(), 130314, 0);

      unsigned char sid;
      unsigned char instance;
      tN2kPressureSource source;
      double pressure;
      CHECK( ParseN2kPGN130314(*m, sid, instance, source, pressure) );
      CHECK( instance == 0 );
      CHECK( source == N2kps_Atmospheric );
      CHECK( pressure == 130314 );
    }
  }

  SECTION("IMU") {
    SKUpdateStatic<2> imuUpdate;
    imuUpdate.setNavigationHeadingMagnetic(1.5708);
    imuUpdate.setNavigationAttitude(SKTypeAttitude(3, 1, 0));

    visitor.processUpdate(imuUpdate);

    CHECK( visitor.getMessages().size() == 2 );
    const tN2kMsg *mAttitude = findMessage(visitor.getMessages(), 127257, 0);
    const tN2kMsg *mHeading = findMessage(visitor.getMessages(), 127250, 0);

    CHECK( mAttitude );
    if (mAttitude) {
      unsigned char sid;
      double yaw;
      double pitch;
      double roll;

      CHECK( ParseN2kPGN127257(*mAttitude, sid, yaw, pitch, roll) );
      CHECK( yaw == 0 );
      CHECK( pitch == 1 );
      CHECK( roll == 3 );
    }

    CHECK( mHeading );
    if (mHeading) {
      unsigned char sid;
      double heading;
      double deviation;
      double variation;
      tN2kHeadingReference ref;

      CHECK( ParseN2kPGN127250(*mHeading, sid, heading, deviation, variation, ref) );
      CHECK( heading == 1.5708 );
      CHECK( deviation == N2kDoubleNA );
      CHECK( variation == N2kDoubleNA );
      CHECK( ref == N2khr_magnetic );
    }
  }

  SECTION("WIND") {
    SKUpdateStatic<2> windUpdate;

    SECTION("Apparent wind angle and apparent wind speed") {
      windUpdate.setEnvironmentWindAngleApparent(SKDegToRad(-30));
      windUpdate.setEnvironmentWindSpeedApparent(SKKnotToMs(4.5));

      visitor.processUpdate(windUpdate);

      CHECK( visitor.getMessages().size() == 1 );

      const tN2kMsg *mWind = findMessage(visitor.getMessages(), 130306, 0);

      CHECK( mWind );
      if (mWind) {
        unsigned char sid;
        double windSpeed;
        double windAngle;
        tN2kWindReference windReference;

        CHECK( ParseN2kPGN130306(*mWind, sid, windSpeed, windAngle, windReference) );

        CHECK( windSpeed == ApproxN2kWindSpeed(SKKnotToMs(4.5)) );
        CHECK( windAngle == ApproxN2kWindAngle(SKDegToRad(360 + -30)) );
        CHECK( windReference == N2kWind_Apparent );
      }
    }

    SECTION("True wind angle relative to boat and speed in water") {
      // Both Raymarine i70 and B&G Vulcan 7 seem to ignore N2kWind_True_water
      windUpdate.setEnvironmentWindAngleTrueWater(SKDegToRad(-30));
      windUpdate.setEnvironmentWindSpeedTrue(SKKnotToMs(4.5));

      visitor.processUpdate(windUpdate);

      CHECK( visitor.getMessages().size() == 1 );

      const tN2kMsg *mWind = findMessage(visitor.getMessages(), 130306, 0);

      CHECK( mWind );
      if (mWind) {
        unsigned char sid;
        double windSpeed;
        double windAngle;
        tN2kWindReference windReference;

        CHECK( ParseN2kPGN130306(*mWind, sid, windSpeed, windAngle, windReference) );

        CHECK( windSpeed == ApproxN2kWindSpeed(SKKnotToMs(4.5)) );
        CHECK( windAngle == ApproxN2kWindAngle(SKDegToRad(360 + -30)) );
        CHECK( windReference == N2kWind_True_water );
      }
    }

    SECTION("True wind angle relative to boat and speed over ground") {
      windUpdate.setEnvironmentWindAngleTrueGround(SKDegToRad(-30));
      windUpdate.setEnvironmentWindSpeedOverGround(SKKnotToMs(4.5));

      visitor.processUpdate(windUpdate);

      CHECK( visitor.getMessages().size() == 1 );

      const tN2kMsg *mWind = findMessage(visitor.getMessages(), 130306, 0);

      CHECK( mWind );
      if (mWind) {
        unsigned char sid;
        double windSpeed;
        double windAngle;
        tN2kWindReference windReference;

        CHECK( ParseN2kPGN130306(*mWind, sid, windSpeed, windAngle, windReference) );

        CHECK( windSpeed == ApproxN2kWindSpeed(SKKnotToMs(4.5)) );
        CHECK( windAngle == ApproxN2kWindAngle(SKDegToRad(360 + -30)) );
        CHECK( windReference == N2kWind_True_boat );
      }
    }

    SECTION("True wind direction magnetic") {
      windUpdate.setEnvironmentWindDirectionMagnetic(SKDegToRad(270));
      windUpdate.setEnvironmentWindSpeedOverGround(SKKnotToMs(4.5));

      visitor.processUpdate(windUpdate);

      CHECK( visitor.getMessages().size() == 1 );

      const tN2kMsg *mWind = findMessage(visitor.getMessages(), 130306, 0);

      CHECK( mWind );
      if (mWind) {
        unsigned char sid;
        double windSpeed;
        double windAngle;
        tN2kWindReference windReference;

        CHECK( ParseN2kPGN130306(*mWind, sid, windSpeed, windAngle, windReference) );

        CHECK( windSpeed == ApproxN2kWindSpeed(SKKnotToMs(4.5)) );
        CHECK( windAngle == ApproxN2kWindAngle(SKDegToRad(270)) );
        CHECK( windReference == N2kWind_Magnetic );
      }
    }

    SECTION("True wind direction true") {
      windUpdate.setEnvironmentWindDirectionTrue(SKDegToRad(270));
      windUpdate.setEnvironmentWindSpeedOverGround(SKKnotToMs(4.5));

      visitor.processUpdate(windUpdate);

      CHECK( visitor.getMessages().size() == 1 );

      const tN2kMsg *mWind = findMessage(visitor.getMessages(), 130306, 0);

      CHECK( mWind );
      if (mWind) {
        unsigned char sid;
        double windSpeed;
        double windAngle;
        tN2kWindReference windReference;

        CHECK( ParseN2kPGN130306(*mWind, sid, windSpeed, windAngle, windReference) );

        CHECK( windSpeed == ApproxN2kWindSpeed(SKKnotToMs(4.5)) );
        CHECK( windAngle == ApproxN2kWindAngle(SKDegToRad(270)) );
        CHECK( windReference == N2kWind_True_North );
      }
    }
  }
}

