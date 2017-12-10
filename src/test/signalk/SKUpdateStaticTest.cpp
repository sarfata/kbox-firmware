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

#include "../KBoxTest.h"
#include "common/signalk/SKUpdateStatic.h"

TEST_CASE("SKUpdateStatic") {
  SKUpdateStatic<2> u;
  CHECK( u.getSize() == 0 );
  CHECK( u[SKPathNavigationPosition] == SKValueNone );

  CHECK( u.getSource() == SKSourceUnknown );
  CHECK( u.getContext() == SKContextSelf );

  SECTION("Add one value") {
    CHECK( u.setValue(SKPathNavigationSpeedOverGround, 3.3) );

    CHECK( u.getSize() == 1 );
    CHECK( u[SKPathNavigationPosition] == SKValueNone );
    CHECK( u[SKPathNavigationSpeedOverGround] == 3.3 );
    CHECK( u.getPath(0) == SKPathNavigationSpeedOverGround );
    CHECK( u.getValue(0) == 3.3 );
  }

  SECTION("Add too many values") {
    CHECK( u.setValue(SKPathNavigationSpeedOverGround, 1.0) );
    CHECK( u.setValue(SKPathNavigationCourseOverGroundTrue, 2.0) );
    CHECK( ! u.setValue(SKPathNavigationPosition, SKTypePosition(2.2, 3.3, 4.4)) );

    CHECK( u.getSize() == 2 );
    CHECK( u.hasNavigationSpeedOverGround() );
    CHECK( u.getNavigationSpeedOverGround() == 1.0 );
    CHECK( ! u.hasNavigationPosition() );
  }

  SECTION("Re-define existing value") {
    u.setValue(SKPathNavigationSpeedOverGround, 1.3);
    u.setValue(SKPathNavigationCourseOverGroundTrue, 3.3);

    CHECK( u.setValue(SKPathNavigationSpeedOverGround, 1.0) );
    CHECK( u.getNavigationSpeedOverGround() == 1.0 );
  }

  SECTION("Add a source") {
    SKSource s = SKSource::sourceForNMEA0183(SKSourceInputNMEA0183_1, "GP", "RMC");
    u.setSource(s);

    CHECK( u.getSource() == s );
  }

  SECTION("Specify a different context") {
    SKContext differentContext("mrn:xxx");

    SKUpdateStatic<3> u(differentContext);
    CHECK( u.getContext() != SKContextSelf );
    CHECK( u.getContext() == differentContext );
  }

  SECTION("SKPathNavigationSpeedOverGround") {
    SKUpdateStatic<1> u;
    CHECK( u.hasNavigationSpeedOverGround() == false);
    CHECK( u.setNavigationSpeedOverGround(1) );
    CHECK( u.hasNavigationSpeedOverGround() );
    CHECK( u.getNavigationSpeedOverGround() == true );
  }

  SECTION("SKPathElectricalBatteriesVoltage") {
    SKUpdateStatic<3> u;

    CHECK( u.hasElectricalBatteriesVoltage("engine") == false );
    CHECK( u.setElectricalBatteriesVoltage("engine", 12.1) == true );
    CHECK( u.setElectricalBatteriesVoltage("house", 12.2) == true );
    CHECK( u.setElectricalBatteriesVoltage("house2", 12.3) == true );

    CHECK( u.setElectricalBatteriesVoltage("house3", 12.4) == false );

    CHECK( u.getElectricalBatteriesVoltage("engine") == 12.1 );
    CHECK( u.getElectricalBatteriesVoltage("house") == 12.2 );
    CHECK( u.getElectricalBatteriesVoltage("house2") == 12.3 );
  }
};

