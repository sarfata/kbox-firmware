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

#include "KommandReader.h"

KommandReader::KommandReader(const uint8_t *buffer, size_t size) : buffer(buffer), size(size) {
  reset();
}

uint16_t KommandReader::getKommandIdentifier() const {
  return buffer[0] + (buffer[1] << 8);
}

uint8_t KommandReader::read8() {
  if (index >= size) {
    return 0;
  }
  return buffer[index++];
}

uint16_t KommandReader::read16() {
  if (index + 1 >= size) {
    return 0;
  }
  uint16_t w = buffer[index] + (buffer[index + 1] << 8);
  buffer += 2;
  return w;
}

uint32_t KommandReader::read32() {
  if (index + 3 >= size) {
    return 0;
  }
  uint32_t w = buffer[index] + (buffer[index +1 ] << 8) + (buffer[index + 2] << 16) + (buffer[index + 3] << 24);
  index += 4;
  return w;
}

void KommandReader::reset() {
  index = 2;
}

size_t KommandReader::dataSize() const {
  if (size > 2) {
    return size - 2;
  }
  else {
    return 0;
  }
}
