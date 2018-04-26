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

#pragma once

#include <stdint.h>
#include <WString.h>

/**
 * Representation of UTC time.
 *
 * SignalK specifies that timestamp are represented in RFC3339 format, always
 * in UTC. The spec uses the regexp `.*Z`. We chose to use a unix timestamp and
 * to add fractional milliseconds on top.
 *
 */
class SKTime {
  private:
    uint32_t _timestamp;
    uint16_t _milliseconds;
    static const uint16_t unknownMilliseconds = 1042;

  public:
    SKTime() : _timestamp(0), _milliseconds(0) {};
    SKTime(uint32_t time) : _timestamp(time), _milliseconds(unknownMilliseconds) {};
    SKTime(uint32_t time, uint16_t ms) : _timestamp(time), _milliseconds(ms) {
      if (_milliseconds >= 1000) {
        _milliseconds = unknownMilliseconds;
      }
    };

    /**
     * SKTime object from two strings for date and time.
     *
     * @param date ddmmyy
     * @param time hhmmss.ss (utc)
     */
    SKTime(String date, String time);

    bool operator==(const SKTime& v) const;

    bool operator!=(const SKTime& v) const;

    uint32_t getTime() const {
      return _timestamp;
    };

    bool hasMilliseconds() const {
      return _milliseconds != unknownMilliseconds;
    };

    uint16_t getMilliseconds() const {
      if (_milliseconds == unknownMilliseconds) {
        return 0;
      }
      else {
        return _milliseconds;
      }
    };

    String toString() const;
};

