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

#include "WallClock.h"

WallClock wallClock;

WallClock::WallClock() : _timeWasSet(false), _millisecondsOfLastKnownTime(0), _millisecondsProvider(nullptr) {

}

uint32_t WallClock::currentMilliseconds() const {
  if (_millisecondsProvider) {
    return _millisecondsProvider();
  }
  else {
    return 0;
  }
}

void WallClock::setMillisecondsProvider(WallClock::millisecondsProvider_t millisecondsProvider) {
  _millisecondsProvider = millisecondsProvider;

  _millisecondsOfLastKnownTime = currentMilliseconds();
  _lastKnownTime = SKTime();
  _timeWasSet = false;
}

void WallClock::setTime(const SKTime &t) {
  _millisecondsOfLastKnownTime = currentMilliseconds();
  _lastKnownTime = t;
  _timeWasSet = true;
}

const SKTime WallClock::now() const {
  uint32_t nowMs = currentMilliseconds();
  uint32_t deltaMs;

  if (_millisecondsOfLastKnownTime > nowMs) {
    deltaMs = UINT32_MAX - _millisecondsOfLastKnownTime + 1 + nowMs;
  }
  else {
    deltaMs = nowMs - _millisecondsOfLastKnownTime;
  }

  SKTime t = _lastKnownTime + deltaMs;

  return t;
}

bool WallClock::isTimeSet() const {
  return _timeWasSet;
}