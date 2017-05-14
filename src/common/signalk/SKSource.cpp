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

#include "SKSource.h"

static const String typeNMEA0183 = "NMEA0183";
static const String typeNMEA2000 = "NMEA2000";
static const String typeUnknown = "unknown";

static const String labelNMEA1 = "kbox.nmea0183.1";
static const String labelNMEA2 = "kbox.nmea0183.2";
static const String labelNMEA2k = "kbox.nmea2000";
static const String labelUnknown = "kbox.unknown";

const SKSource SKSourceUnknown = SKSource::unknownSource();

SKSource SKSource::unknownSource() {
  SKSource s;
  s._input = SKSourceInputUnknown;
  return s;
}

SKSource SKSource::sourceForNMEA0183(const SKSourceInput input, const String& talker, const String& sentence) {
  SKSource s;
  if (input == SKSourceInputNMEA0183_1 || input == SKSourceInputNMEA0183_2) {
    s._input = input;
  }
  else {
    s._input = SKSourceInputUnknown;
  }
  s._talker = talker;
  s._sentence = sentence;
  return s;
}

bool SKSource::operator==(const SKSource &other) const {
  if (_input != other._input) {
    return false;
  }

  switch (_input) {
    case SKSourceInputNMEA0183_1:
    case SKSourceInputNMEA0183_2:
      return _talker == other._talker && _sentence == other._sentence;
    case SKSourceInputNMEA2000:
      return _pgn == other._pgn;
    case SKSourceInputUnknown:
      return true;
    default:
      return true;
  }
}

bool SKSource::operator!=(const SKSource &other) const {
  return !(*this == other);
}

const String& SKSource::getType() const {
  switch (_input) {
    case SKSourceInputNMEA0183_1:
    case SKSourceInputNMEA0183_2:
      return typeNMEA0183;
    case SKSourceInputNMEA2000:
      return typeNMEA2000;
    default:
      return typeUnknown;
  }
}

const String& SKSource::getTalker() const {
  return _talker;
}

const String& SKSource::getSentence() const {
  return _sentence;
}

const String& SKSource::getLabel() const {
  switch (_input) {
    case SKSourceInputNMEA0183_1:
      return labelNMEA1;
    case SKSourceInputNMEA0183_2:
      return labelNMEA2;
    case SKSourceInputNMEA2000:
      return labelNMEA2k;
    default:
      return labelUnknown;
  }
}

