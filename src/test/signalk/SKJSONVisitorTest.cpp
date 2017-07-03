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
  SKJSONVisitor jsonVisitor(jsonBuffer);
  SKUpdateStatic<5> update;

  SECTION("empty update") {
    JsonObject &o = jsonVisitor.processUpdate(update);

    CHECK( o.containsKey("context") );
    CHECK( o.containsKey("updates") );

    if (o.containsKey("updates") && o.is<JsonArray>("updates")) {
      JsonArray &a = o["updates"];

      CHECK(a.size() == 0);
    }
  }

  SECTION("update with one key") {
    update.setElectricalBatteriesVoltage("starter", 12.0);

    JsonObject &o = jsonVisitor.processUpdate(update);

    CHECK( o.containsKey("updates") );

    JsonObject &voltageUpdate = o["updates"][0];
    CHECK(voltageUpdate != JsonObject::invalid());

    if (voltageUpdate.containsKey("path")) {
      CHECK(voltageUpdate["path"].as<std::string>() == "electrical.batteries.starter.voltage");
    }
    else {
      CHECK(false);
    }
    if (voltageUpdate.containsKey("value")) {
      CHECK(voltageUpdate["value"] == 12.0);
    }
    else {
      CHECK(false);
    }
  }
}
