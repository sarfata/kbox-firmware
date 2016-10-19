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

#include "signalk-test.h"
#include "signalk.h"
#include "signalk-json.h"
#include "picojson.h"

using picojson::object;
using picojson::array;

static const char *vesselName = "La Calypso";

std::string outBuffer;

void printer(const char *s) {
  outBuffer += s;
}

TEST_CASE("empty context") {
  SKContext *ctx = signalk_alloc(vesselName, 0);
  SKJSON *json = signalk_json_alloc(printer);
  outBuffer = "";

  WHEN("in full mode") {
    signalk_json_export_full(json, ctx);
    picojson::value v;
    std::string err = picojson::parse(v, outBuffer);
    REQUIRE( err.empty() );
    REQUIRE( v.is<object>() );
    REQUIRE( v.get<object>()["vessels"].is<object>() );
    REQUIRE( v.get<object>()["version"].to_str() == "1.0" );
  }

  WHEN("in delta mode") {
    signalk_json_export_delta(json, ctx);
    picojson::value v;
    std::string err = picojson::parse(v, outBuffer);
    REQUIRE( outBuffer == "{\"updates\":[]}" );
    REQUIRE( err.empty() );
    REQUIRE( v.is<object>() );
    REQUIRE( v.get<object>()["updates"].is<array>() );
    REQUIRE( v.get<object>()["updates"].get<array>().size() == 0 );
  }
}
