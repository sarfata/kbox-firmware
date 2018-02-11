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

#include <ArduinoJson.h>
#include "common/signalk/SKUpdateStatic.h"
#include "common/signalk/SKJSONVisitor.h"
#include "../KBoxTest.h"

TEST_CASE("SKJSONVisitor") {
  DynamicJsonBuffer jsonBuffer;
  SKJSONVisitor jsonVisitor("urn:mrn:kbox:unit-test", jsonBuffer);
  SKUpdateStatic<5> update;

  SECTION("empty update") {
    JsonObject &o = jsonVisitor.processUpdate(update);

    CHECK( o.containsKey("context") );
    CHECK( o.containsKey("updates") );
    CHECK( o["updates"].is<JsonArray>() );

    CHECK( o["updates"].size() == 1 );
    JsonObject &u = o["updates"][0];

    CHECK( u.containsKey("source") );
    CHECK( !u.containsKey("timestamp") );
    CHECK( u.containsKey("values") );
    CHECK( u["values"].as<JsonArray>().size() == 0 );
  }

  SECTION("empty update with a timestamp") {
    update.setTimestamp(SKTime(409516200));

    JsonObject &o = jsonVisitor.processUpdate(update);

    CHECK( o["updates"][0].is<JsonObject>() );
    CHECK( o["updates"][0]["timestamp"] == "1982-12-23T18:30:00Z" );
  }

  SECTION("update from one source at one time") {
    update.setElectricalBatteriesVoltage("starter", 12.0);
    update.setTimestamp(SKTime(409516200));

    JsonObject &o = jsonVisitor.processUpdate(update);

    CHECK( o.containsKey("updates") );
    CHECK( o["updates"].is<JsonArray>() );

    JsonArray &updates = o["updates"].as<JsonArray>();

    CHECK( updates.size() == 1 );
    CHECK( updates[0].is<JsonObject>() );

    JsonObject &update = updates[0].as<JsonObject&>();

    CHECK( update.containsKey("source") );
    CHECK( update.containsKey("timestamp") );
    CHECK( update.containsKey("values") );

    JsonObject &voltageUpdate = update["values"][0];
    CHECK(voltageUpdate != JsonObject::invalid());
    CHECK(voltageUpdate["path"].as<std::string>() == "electrical.batteries.starter.voltage");
    CHECK(voltageUpdate["value"] == 12.0);
  }
}
