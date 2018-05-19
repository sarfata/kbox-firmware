/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

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

#include <cstdint>
#include "../signalk/SKTime.h"

/**
 * Implements a time keeper with milliseconds() resolution.
 *
 * The standard Arduino timelib does not provide milliseconds resolution.
 */
class WallClock {
  public:
    typedef uint32_t (*millisecondsProvider_t)();

  private:
    bool _timeWasSet;
    SKTime _lastKnownTime;
    uint32_t _millisecondsOfLastKnownTime;

    millisecondsProvider_t _millisecondsProvider;

    uint32_t currentMilliseconds() const;

  public:
    WallClock();

    void setMillisecondsProvider(WallClock::millisecondsProvider_t millisecondsProvider);
    void setTime(const SKTime &t);

    /**
     * Return the current time.
     *
     * If the time was not set, this will probably be in 1970.
     * If a milliseconds provider was not set, the time will never be updated.
     *
     * @return
     */
    const SKTime now() const;

    /**
     * Returns true if the time has been set at least once and is therefore believed to be correct.
     *
     * @return
     */
    bool isTimeSet() const;
};

extern WallClock wallClock;

