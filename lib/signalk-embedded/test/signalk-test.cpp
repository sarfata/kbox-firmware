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
#include "signalk.h"

static const char *vesselName = "test-vessel";
static const char *vesselMMSI = "001-420-410";

TEST_CASE("Creating a context") {

  WHEN("standard case") {
    SKContext *ctx = signalk_alloc(vesselName, vesselMMSI);
    REQUIRE(ctx != NULL);
    signalk_release(ctx);
  }

  WHEN("no vessel name") {
    SKContext *ctx = signalk_alloc(NULL, vesselMMSI);
    REQUIRE(ctx != NULL);
    signalk_release(ctx);
  }

  WHEN("no vessel name or mmsi") {
    SKContext *ctx = signalk_alloc(NULL, NULL);
    REQUIRE(ctx == NULL);
    signalk_release(ctx);
    // Should not crash.
  }
};

TEST_CASE("Getting things") {
  SKContext *ctx = signalk_alloc(vesselName, vesselMMSI);

  SKSource s;
  SKValue v;

  WHEN("call get without a context") {
    int r = signalk_get(NULL, SKPathForKey(SKKeyVesselName), NULL, NULL);
    REQUIRE(r == SKErrorInvalidArg);
  }

  WHEN("getting vesselname") {
    int r = signalk_get(ctx, SKPathForKey(SKKeyVesselName), &s, &v);
    REQUIRE(r == SKErrorOK);
    REQUIRE(std::string(v.v.stringValue) == vesselName);
    REQUIRE(s.type == SKSourceKBox);
  }

  WHEN("getting vessel mmsi") {
    int r = signalk_get(ctx, SKPathForKey(SKKeyVesselMMSI), &s, &v);
    REQUIRE(r == SKErrorOK);
    REQUIRE(std::string(v.v.stringValue) == vesselMMSI);
    REQUIRE(s.type == SKSourceKBox);
  }

  WHEN("call get with null for source") {
    int r = signalk_get(ctx, SKPathForKey(SKKeyVesselName), NULL, &v);
    REQUIRE(r == SKErrorOK);
    REQUIRE(std::string(v.v.stringValue) == vesselName);
  }

  WHEN("call get for a key that has not been defined yet") {
    int r = signalk_get(ctx, SKPathForKey(SKKeyNavigationPosition), &s, &v);
    REQUIRE(r == SKErrorUndefinedValue);
  }

  WHEN("call get for a key that requires an instance when it has not been defined") {
    int r = signalk_get(ctx, SKPathForKeyWithInstance(SKKeyElectricalBatteryInstance, "1"), &s, &v);
    REQUIRE(r == SKErrorUndefinedValue);
  }

  WHEN("call get for a key that requires an instance when it has been defined") {
    v = SKValueMakeBattery(12.0, 0.3, 323);
    s = SKSourceMakeNMEA2000(127508, "22");

    int r = signalk_set(ctx, SKPathForKeyWithInstance(SKKeyElectricalBatteryInstance, "1"), &s, &v);
    CHECK(r == SKErrorOK);

    SKSource newSource;
    SKValue newValue;
    r = signalk_get(ctx, SKPathForKeyWithInstance(SKKeyElectricalBatteryInstance, "1"), &newSource, &newValue);
    REQUIRE(r == SKErrorOK);

    REQUIRE(newSource.type == s.type);
    REQUIRE(newSource.pgn == s.pgn);
    REQUIRE(std::string(newSource.src) == s.src);

    REQUIRE(newValue.v.battery.dc.voltage == v.v.battery.dc.voltage);
    REQUIRE(newValue.v.battery.dc.current == v.v.battery.dc.current);
    REQUIRE(newValue.v.battery.dc.temperature == v.v.battery.dc.temperature);
  }

  WHEN("call get for a key that requires an instance without passing instance") {
    int r = signalk_get(ctx, SKPathForKey(SKKeyElectricalBatteryInstance), &s, &v);
    REQUIRE(r == SKErrorInvalidPath);
  }

  WHEN("call get with an instance for a key that does not take an instance") {
    int r = signalk_get(ctx, SKPathForKeyWithInstance(SKKeyNavigationPosition, "1"), &s, &v);
    REQUIRE(r == SKErrorInvalidPath);
  }

  WHEN("call get with an invalid key") {
    int r = signalk_get(ctx, SKPathForKey((SKKey)2132), &s, &v);
    REQUIRE(r == SKErrorInvalidPath);
    r = signalk_get(ctx, SKPathForKey(SKKeyRequireInstanceMarker), &s, &v);
    REQUIRE(r == SKErrorInvalidPath);
  }
}

TEST_CASE("Setting things") {
  WHEN("Setting for a key without instance") {
  }
  WHEN("Setting for a key with an instance") {
  }

  WHEN("forgetting to specify the instance when it's required") {
  }

  WHEN("specifying an instance when it's not needed") {
  }

}

TEST_CASE("Update callback") {
}
