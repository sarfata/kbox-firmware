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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <stdarg.h>
#define DEBUG(...) debug(__FILE__, __LINE__, __VA_ARGS__)

void debug(const char *fname, int lineno, const char *fmt, ... ) {
  printf("%s: %i ", fname, lineno);
  char tmp[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(tmp, 128, fmt, args);
  va_end (args);
  printf("%s\n", tmp);
}

#include "util/SlipStream.h"

struct rxBuffer {
  size_t len;
  size_t index;
  const uint8_t *data;
};

class StreamMock : public Stream {
  private:
    // For mock purposes, we have a number of rxBuffers, initially, only
    // the first one will be delivered, then the next one, etc.
    unsigned int countBuffers = 0;
    unsigned int currentBuffer = 0;
    struct rxBuffer *rxBuffers = 0;

    struct rxBuffer *getCurrentBuffer() {
      if (currentBuffer < countBuffers) {
        return &rxBuffers[currentBuffer];
      }
      else {
        return 0;
      }
    };

  public:
    StreamMock() {};

    StreamMock(int count, struct rxBuffer *b) : countBuffers(count), rxBuffers(b) {
    };

    void advanceToNextBuffer() {
      currentBuffer++;
    };

    int available() {
      struct rxBuffer *b = getCurrentBuffer();

      if (b) {
        return b->len;
      }
      else {
        return 0;
      }
    };

    int read() {
      struct rxBuffer *b = getCurrentBuffer();

      if (b && b->index < b->len) {
        return b->data[b->index++];
      }
      else {
        return -1;
      }
    };

    int peek() {
      struct rxBuffer *b = getCurrentBuffer();

      if (b && b->index < b->len) {
        return b->data[b->index];
      }
      else {
        return -1;
      }
    };

    size_t write(uint8_t b) {
      return 1;
    };

    void flush() {
      // nop
    };
};

TEST_CASE("empty messages") {
  WHEN("reading empty stream") {
    StreamMock emptyStream(0, 0);
    SlipStream slip(emptyStream, 100);

    REQUIRE( slip.available() == 0 );

    uint8_t ptr[100];
    REQUIRE( slip.read(ptr, sizeof(ptr)) == 0);
  }

  WHEN("reading EOM byte repeatedly") {
    struct rxBuffer b;
    b.len = 10;
    b.data = (const uint8_t*)"\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0\xc0";

    StreamMock eomBytesStream(1, &b);
    SlipStream slip(eomBytesStream, 100);

    REQUIRE( slip.available() == 0 );
  }
}

TEST_CASE("data read in one read-call") {
  uint8_t ptr[100];

  WHEN("reading actual data") {
    struct rxBuffer b;
    b.len = 10;
    b.data = (const uint8_t*)"1234567890";

    StreamMock eomBytesStream(1, &b);
    SlipStream slip(eomBytesStream, 100);

    REQUIRE( slip.available() == 10 );
    REQUIRE( slip.read(ptr, sizeof(ptr)) == 10);
    REQUIRE( memcmp(ptr, b.data, 10) == 0 );
  }

  WHEN("reading actual data with escaping") {
    struct rxBuffer b;
    b.len = 9;
    b.data = (const uint8_t*)"1:\xdb\xdc 2:\xdb\xdd";
    const uint8_t *expected = (const uint8_t*)"1:\xc0 2:\xdb";

    StreamMock eomBytesStream(1, &b);
    SlipStream slip(eomBytesStream, 100);

    REQUIRE( slip.available() == 7 );
    REQUIRE( slip.read(ptr, sizeof(ptr)) == 7);
    REQUIRE( memcmp(ptr, expected, 7) == 0 );
  }
}




// I have not found a way to get WString.cpp to compile on a Linux box without modifying it.
// Would require some serious modifications because it relies on non-std functions like
// itoa, utoa, etc that are AVR specific apparently and not available here.
// like itoa, utoa, etc.
// So it's a lot simpler to copy here the one function we need.

void String::getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index) const
{
  if (!bufsize || !buf) return;
  if (index >= len) {
    buf[0] = 0;
    return;
  }
  unsigned int n = bufsize - 1;
  if (n > len - index) n = len - index;
  strncpy((char *)buf, buffer + index, n);
  buf[n] = 0;
}

