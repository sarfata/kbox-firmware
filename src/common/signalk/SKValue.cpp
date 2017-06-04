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

const SKValue SKValueNone = SKValue();

bool SKValue::operator==(const SKValue& other) const {
  if (_type == other._type) {
    switch (_type) {
      case SKValueTypeNone:
        return true;
      case SKValueTypeNumber:
        return _value.numberValue == other._value.numberValue;
      case SKValueTypePosition:
        return (_value.position.latitude == other._value.position.latitude) &&
          (_value.position.longitude == other._value.position.longitude) &&
          (_value.position.altitude == other._value.position.altitude);
    }
  }
  return false;
}

bool SKValue::operator!=(const SKValue& other) const {
  return !(*this == other);
}

double SKValue::getNumberValue() const {
  if (_type != SKValueTypeNumber) {
    return 0;
  }
  return _value.numberValue;
}

SKTypePosition SKValue::getPositionValue() const {
  if (_type != SKValueTypePosition) {
    return SKTypePosition(0,0,0);
  }
  return _value.position;
}
