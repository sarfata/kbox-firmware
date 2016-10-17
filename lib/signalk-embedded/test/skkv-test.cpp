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
#include "skkv.h"
#include "skkv-private.h"

TEST_CASE("count empty list") {
  REQUIRE(skkv_count(0) == 0);
}

TEST_CASE("insert operations") {
  SKSource s = SKSourceMakeNMEA2000(42, "xx");
  SKValue v = SKValueMakeString("a new string");

  WHEN("inserting into empty list") {
    SKKV *head = skkv_set(0, SKPathForKey(SKKeyVesselName), &s, &v);
    REQUIRE(head != 0);
    REQUIRE(head->next == 0);
    REQUIRE(skkv_count(head) == 1);
  }

  WHEN("inserting into existing list") {
    SKKV *head = skkv_set(0, SKPathForKey(SKKeyVesselName), &s, &v);
    head = skkv_set(head, SKPathForKey(SKKeyVesselMMSI), &s, &v);

    REQUIRE(head != 0);
    REQUIRE(head->next != 0);
    REQUIRE(head->next->next == 0);
    REQUIRE(skkv_count(head) == 2);
  }

  WHEN("replacing existing element") {
    SKKV *head = skkv_set(0, SKPathForKey(SKKeyVesselName), &s, &v);
    head = skkv_set(head, SKPathForKey(SKKeyVesselName), &s, &v);

    REQUIRE(head != 0);
    REQUIRE(head->next == 0);
    REQUIRE(skkv_count(head) == 1);
  }

}

