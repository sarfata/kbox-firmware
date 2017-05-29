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

#include <string.h>
#include "signalk-test.h"
#include "signalk-path.h"

TEST_CASE("signalk_path_cmp") {
  WHEN("vessel is null and no instance") {
    REQUIRE(signalk_path_cmp(SKPathForKey(SKKeyVesselName), SKPathForKey(SKKeyVesselName)) == 0);
    REQUIRE(signalk_path_cmp(SKPathForKey(SKKeyVesselName), SKPathForKey(SKKeyElectricalBatteryInstance)) < 0);
    REQUIRE(signalk_path_cmp(SKPathForKey(SKKeyElectricalBatteryInstance), SKPathForKey(SKKeyVesselName)) > 0);
  }

  WHEN("vessel is null and instance provided") {
    REQUIRE(signalk_path_cmp(SKPathForKeyWithInstance(SKKeyElectricalBatteryInstance, "1"), SKPathForKeyWithInstance(SKKeyElectricalBatteryInstance, "2")) < 0);
    REQUIRE(signalk_path_cmp(SKPathForKeyWithInstance(SKKeyElectricalBatteryInstance, "1"), SKPathForKeyWithInstance(SKKeyElectricalBatteryInstance, "1")) == 0);
    REQUIRE(signalk_path_cmp(SKPathForKeyWithInstance(SKKeyElectricalBatteryInstance, "2"), SKPathForKeyWithInstance(SKKeyElectricalBatteryInstance, "1")) > 0);
  }

  WHEN("comparing with vessel to no vessel") {
    SKPath p = SKPathForKey(SKKeyVesselName);
    p.vessel = "calypso";

    REQUIRE(signalk_path_cmp(p, SKPathForKey(SKKeyVesselName)) > 0);
    REQUIRE(signalk_path_cmp(SKPathForKey(SKKeyVesselName), p) < 0);
  }

  WHEN("comparing with vessel to same vessel") {
    SKPath p = SKPathForKey(SKKeyVesselName);
    p.vessel = "calypso";
    REQUIRE(signalk_path_cmp(p, p) == 0);

    SKPath p2 = p;
    p2.vessel = strdup(p.vessel);
    REQUIRE(signalk_path_cmp(p, p2) == 0);
    free((void*)p2.vessel);

    p2.vessel = p.vessel;
    p2.key = SKKeyElectricalBatteryInstance;
    REQUIRE(signalk_path_cmp(p, p2) < 0);
  }

  WHEN("comparing with vessel to another vessel") {
    SKPath p = SKPathForKey(SKKeyVesselName);
    p.vessel = "calypso";
    SKPath p2 = SKPathForKey(SKKeyElectricalBatteryInstance);
    p2.vessel = "albatros";

    REQUIRE(signalk_path_cmp(p, p2) > 0);
  }
}

