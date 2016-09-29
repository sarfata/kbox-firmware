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

#include <Stream.h>
#include <KBoxDebug.h>
#include "SlipStream.h"

SlipStream::SlipStream(Stream &s, size_t mtu) : _stream(s), _mtu(mtu) {
  buffer = (uint8_t*)malloc(mtu);
  index = 0;
}

size_t SlipStream::available() {
  while (_stream.available() > 0 && index < _mtu && !messageComplete) {
    uint8_t b = _stream.read();

    if (escapeMode) {
      if (b == 0xdc) {
        buffer[index++] = 0xc0;
      }
      else if (b == 0xdd) {
        buffer[index++] = 0xdb;
      }
      else {
        // escaping error!
        index = 0;
        _invalidFrameErrors++;
        _invalidFrame = true;
      }
      escapeMode = false;
    }
    else {
      if (b == 0xc0) {
        if (_invalidFrame) {
          _invalidFrame = false;
          index = 0;
        }
        else if (index > 0) {
          messageComplete = true;
        }
      }
      else if (b == 0xdb) {
        escapeMode = true;
      }
      else {
        buffer[index++] = b;
      }
    }
  }

  // Reject frames that are greater than mtu
  if (!messageComplete && index >= _mtu) {
    index = 0;
    _invalidFrameErrors++;
    _invalidFrame = true;
  }

  if (messageComplete) {
    return index;
  } else {
    return 0;
  }
}

size_t SlipStream::readFrame(uint8_t *ptr, size_t len) {
  if (len > index) {
    len = index;
  }
  memcpy(ptr, buffer, len);
  messageComplete = false;
  index = 0;
  return len;
}

size_t SlipStream::writeFrame(const uint8_t *ptr, size_t len) {
  _stream.write(0xc0);
  for (size_t i = 0; i < len; i++) {
    if (ptr[i] == 0xc0) {
      _stream.write(0xdb);
      _stream.write(0xdc);
    }
    else if (ptr[i] == 0xdb) {
      _stream.write(0xdb);
      _stream.write(0xdd);
    }
    else {
      _stream.write(ptr[i]);
    }
  }
  _stream.write(0xc0);
  return len;
}

