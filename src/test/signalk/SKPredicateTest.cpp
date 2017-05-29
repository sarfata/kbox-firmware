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
#include "common/signalk/SKPredicate.h"
#include "common/signalk/SKUpdateStatic.h"

TEST_CASE("SKPredicate") {
  SKUpdateStatic<2> update;

  SECTION("SKContextPredicate") {
    SKContextPredicate p = SKContextPredicate(SKContextSelf);

    CHECK( p.evaluate(update) );

    SKContext otherBoatContext("mrn:someotherboat");
    SKUpdateStatic<0> otherUpdate(otherBoatContext);

    CHECK( !p.evaluate(otherUpdate) );
  }

  SECTION("SKSourceInputPredicate") {
    SKSourceInputPredicate p = SKSourceInputPredicate(SKSourceInputNMEA0183_1);

    update.setSource(SKSource::sourceForNMEA0183(SKSourceInputNMEA0183_1, "GP", "RMC"));
    CHECK( p.evaluate(update) );

    update.setSource(SKSource::unknownSource());
    CHECK( ! p.evaluate(update) );
  }

  SECTION("SKNotPredicate") {
    SKNotPredicate p = SKNotPredicate(SKContextPredicate(SKContextSelf));

    CHECK( ! p.evaluate(update) );
  }

  SECTION("SKAndPredicate") {
    SKAndPredicate p = SKAndPredicate(SKContextPredicate(SKContextSelf), SKSourceInputPredicate(SKSourceInputNMEA0183_2));
    update.setSource(SKSource::sourceForNMEA0183(SKSourceInputNMEA0183_2, "GP", "RMC"));

    CHECK( p.evaluate(update) );

    SKUpdateStatic<2> update2(SKContext("mrn:other"));
    update2.setSource(SKSource::sourceForNMEA0183(SKSourceInputNMEA0183_2, "GP", "RMC"));

    CHECK( ! p.evaluate(update2) );
  }
};
