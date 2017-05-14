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

#include "SKValue.h"

const SKValue SKValueNone = SKValue::noneValue();

bool SKValue::operator==(const SKValue& other) const {
  if (_path == other._path) {
    switch (_path) {
      case SKPathNavigationCourseOverGroundTrue:
      case SKPathNavigationSpeedOverGround:
        return _value.doubleValue == other._value.doubleValue;
      case SKPathNavigationPosition:
        return (_value.position.latitude == other._value.position.latitude) &&
          (_value.position.longitude == other._value.position.longitude);
      case SKPathInvalid:
        return true;
      default:
        return false;
    }
  }
  else {
    return false;
  }
}

bool SKValue::operator!=(const SKValue& other) const {
  return !(*this == other);
}
