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

#include <stdlib.h>
#include "signalk-json.h"
#include "signalk.h"

struct SKJSON {
  SKJSONPrintCallback print;
};

SKJSON *signalk_json_alloc(SKJSONPrintCallback callback) {
  SKJSON *json = malloc(sizeof(SKJSON));
  json->print = callback;
  return json;
}

void signalk_json_release(SKJSON *json) {
  if (json) {
    free(json);
  }
}

void signalk_json_export_full(SKJSON *json, SKContext *ctx) {
  json->print("{\"vessels\":{\"");

  SKValue v;

  signalk_get(ctx, SKPathForKey(SKKeyVesselName), NULL, &v);
  json->print(v.v.stringValue);

  json->print("\":{}},\"version\":\"1.0\"}");
}

void signalk_json_export_delta(SKJSON *json, SKContext *update) {
  json->print("{\"updates\":[]}");
}
