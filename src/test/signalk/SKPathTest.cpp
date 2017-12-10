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

#include "../KBoxTest.h"
#include "common/signalk/SKPath.h"

TEST_CASE("SKPath") {
  SECTION("non-indexed paths") {
    // Implicitly casting a SKPathEnum value to a SKPath should work
    SKPath p = SKPathNavigationSpeedOverGround;
    // and == too
    CHECK( p == SKPathNavigationSpeedOverGround );

    CHECK( p.toString() == "navigation.speedOverGround" );
  }

  SECTION("indexed paths") {
    // Invalid assignment because no index provided
    SKPath p = SKPathElectricalBatteriesVoltage;
    CHECK( p == SKPathInvalid );
    CHECK( ! p.isIndexed() );

    p = SKPath(SKPathElectricalBatteriesVoltage, "starter");

    SKPath p2 = SKPath(SKPathElectricalBatteriesVoltage, "engine");
    CHECK( p != p2 );
    CHECK( p2.isIndexed() );
    CHECK( p2.getIndex() == "engine" );
    CHECK( p2.getStaticPath() == SKPathElectricalBatteriesVoltage );
    CHECK( p2.toString() == "electrical.batteries.engine.voltage" );

    SKPath p3 = SKPath(SKPathElectricalBatteriesVoltage, "starter");
    CHECK( p == p3 );
  }
}

