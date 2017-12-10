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

#pragma once

#include <Stream.h>

class SlipStream {
  private:
    Stream &_stream;
    size_t _mtu;
    uint8_t *buffer = 0;
    uint32_t index = 0;
    bool escapeMode = false;
    bool messageComplete = false;

    bool _invalidFrame = true;
    uint32_t _invalidFrameErrors = 0;

  public:
    SlipStream(Stream &s, size_t mtu);

    /**
     * Returns 0 if no complete message has been received.
     * Returns the size of a message if a full message has been received.
     */
    size_t available();

    /**
     * Copies received message into given buffer and then discards it.
     *
     * Returns number of bytes copied in the buffer. If the buffer is too
     * small, only the first len bytes will be copied and then the entire
     * message will be discarded.
     */
    size_t readFrame(uint8_t *ptr, size_t len);

    /**
     * Allows caller to peek at current frame and get access to the internal
     * buffer where it is stored.
     * Caller needs to call readFrame() afterwards to discard the frame and
     * start receiving another one. Note that readFrame(0,0) is valid.
     *
     * @param out ptr: Pointer will be updated to point to internal buffer.
     * @return number of bytes that can be read in buffer or 0 if buffer is
     * invalid.
     */
    size_t peekFrame(uint8_t **ptr);

    /**
     * Writes a frame to the underlying Stream, using SLIP framing and
     * escaping.
     */
    size_t writeFrame(const uint8_t *ptr, size_t len);

    /**
     * Returns the number of invalid frames that were rejected.
     */
    uint32_t invalidFrameErrors() const {
      return _invalidFrameErrors;
    }
};
