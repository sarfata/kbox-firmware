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

/*
 * This test executes full conversion of NMEA sentences into SignalK and then
 * back into NMEA2000 messages and validates that the output matches what is
 * expected.
 */

#include <KBoxLogging.h>
#include "../KBoxTest.h"
#include "common/comms/KommandReader.h"

TEST_CASE("KommandReader") {
  SECTION("Empty kommand") {
    const uint8_t frame[] = {};

    KommandReader kr = KommandReader(frame, sizeof(frame));

    CHECK(kr.getKommandIdentifier() == 0);
    CHECK(kr.dataSize() == 0);
    CHECK(kr.dataIndex() == 0);

    CHECK(kr.read8() == 0);
    CHECK(kr.read16() == 0);
    CHECK(kr.read32() == 0);
    CHECK(kr.readNullTerminatedString() == nullptr);

    CHECK(kr.dataIndex() == 0);
  }

  SECTION("Kommand with some data") {
    const uint8_t frame[] = {0x2a, 0x00,
                             0x01,
                             0x01, 0x02,
                             0x01, 0x02, 0x03, 0x04,
                             'a', 'b', 'c', '\0'
    };

    KommandReader kr = KommandReader(frame, sizeof(frame));

    CHECK(kr.getKommandIdentifier() == 42);
    CHECK(kr.dataSize() == 11);
    CHECK(kr.dataIndex() == 0);

    CHECK(kr.read8() == 1);
    CHECK(kr.dataIndex() == 1);

    CHECK(kr.read16() == 0x0201);
    CHECK(kr.dataIndex() == 3);

    CHECK(kr.read32() == 0x04030201);
    CHECK(kr.dataIndex() == 7);

    auto originalString = reinterpret_cast<const char *>(&frame[9]);
    auto readString = kr.readNullTerminatedString();
    CHECK( readString == originalString );

    CHECK(kr.dataIndex() == 11);
  }

  SECTION("Kommand with invalid string") {
    const uint8_t frame[] = {0x2a, 0x00,
                             'a', 'b', 'c', 'd'
    };

    KommandReader kr = KommandReader(frame, sizeof(frame));
    CHECK( kr.readNullTerminatedString() == 0 );
  }

  SECTION("Kommand with two strings") {
    const uint8_t frame[] = {0x2a, 0x00,
                             'a', 'b', 'c', 'd', '\0',
                             'd', 'e', 'f', 'g', '\0'
    };

    KommandReader kr = KommandReader(frame, sizeof(frame));
    CHECK( kr.readNullTerminatedString() == reinterpret_cast<const char *>
                                            (&frame[2]) );
    CHECK( kr.readNullTerminatedString() == reinterpret_cast<const char *>
                                            (&frame[7]) );
  }

  SECTION("Kommand with three strings - one empty") {
    const uint8_t frame[] = {0x2a, 0x00,
                             'a', 'b', '\0',
                             '\0',
                             'c', 'd', '\0'
    };

    KommandReader kr = KommandReader(frame, sizeof(frame));
    CHECK( kr.readNullTerminatedString() == reinterpret_cast<const char *>
                                            (&frame[2]) );
    CHECK( kr.readNullTerminatedString() == reinterpret_cast<const char *>
                                            (&frame[5]) );
    CHECK( kr.readNullTerminatedString() == reinterpret_cast<const char *>
                                            (&frame[6]) );
  }

}