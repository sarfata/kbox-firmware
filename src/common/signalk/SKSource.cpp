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

#include <common/util/bsd-string.h>
#include "SKSource.h"

static const String typeNMEA0183 = "NMEA0183";
static const String typeNMEA2000 = "NMEA2000";
static const String typeSensor = "InternalSensor";
static const String typePerformance = "Performance Calculation";
static const String typeUnknown = "unknown";

static const String labelNMEA1 = "kbox.nmea0183.1";
static const String labelNMEA2 = "kbox.nmea0183.2";
static const String labelNMEA2k = "kbox.nmea2000";
static const String labelSensor = "kbox.sensor";
static const String labelPerformance = "kbox.performance";
static const String labelUnknown = "kbox.unknown";

const SKSource SKSourceUnknown = SKSource::unknownSource();
const SKSource SKSourceInternalSensor = SKSource::internalSensor();
const SKSource SKSourcePerformanceCalc = SKSource::performanceCalc();

SKSource SKSource::unknownSource() {
  SKSource s;
  s._input = SKSourceInputUnknown;
  return s;
}

SKSource SKSource::internalSensor() {
  SKSource s;
  s._input = SKSourceInputSensor;
  return s;
}

SKSource SKSource::performanceCalc(){
  SKSource s;
  s._input = SKSourceInputPerformance;
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
  strlcpy(s._info.nmea.talker, talker.c_str(), sizeof(s._info.nmea.talker));
  strlcpy(s._info.nmea.sentence, sentence.c_str(), sizeof(s._info.nmea.sentence));
  return s;
}

SKSource SKSource::sourceForNMEA2000(const SKSourceInput input, const uint32_t pgn,
    const unsigned char priority, const unsigned char sourceAddress) {
  SKSource s;

  if (input != SKSourceInputNMEA2000) {
    s._input = SKSourceInputUnknown;
    return s;
  }

  s._input = input;
  s._info.nmea2000.pgn = pgn;
  s._info.nmea2000.priority = priority;
  s._info.nmea2000.sourceAddress = sourceAddress;
  return s;
}

bool SKSource::operator==(const SKSource &other) const {
  if (_input != other._input) {
    return false;
  }

  switch (_input) {
    case SKSourceInputNMEA0183_1:
    case SKSourceInputNMEA0183_2:
      return strcmp(_info.nmea.talker, other._info.nmea.talker) == 0
        && strcmp(_info.nmea.sentence, other._info.nmea.sentence) == 0;
    case SKSourceInputNMEA2000:
      return _info.nmea2000.pgn == other._info.nmea2000.pgn
        && _info.nmea2000.sourceAddress == other._info.nmea2000.sourceAddress
        && _info.nmea2000.priority == other._info.nmea2000.priority;
    case SKSourceInputUnknown:
    case SKSourceInputSensor:
      return true;
    default:
      return true;
  }
}

bool SKSource::operator!=(const SKSource &other) const {
  return !(*this == other);
}

const SKSourceInput& SKSource::getInput() const {
  return _input;
}

const String& SKSource::getType() const {
  switch (_input) {
    case SKSourceInputNMEA0183_1:
    case SKSourceInputNMEA0183_2:
      return typeNMEA0183;
    case SKSourceInputNMEA2000:
      return typeNMEA2000;
    case SKSourceInputSensor:
      return typeSensor;
    case SKSourceInputPerformance:
      return typePerformance;
    default:
      return typeUnknown;
  }
}

const String& SKSource::getLabel() const {
  switch (_input) {
    case SKSourceInputNMEA0183_1:
      return labelNMEA1;
    case SKSourceInputNMEA0183_2:
      return labelNMEA2;
    case SKSourceInputNMEA2000:
      return labelNMEA2k;
    case SKSourceInputSensor:
      return labelSensor;
    case SKSourceInputPerformance:
      return labelPerformance;
    default:
      return labelUnknown;
  }
}

const char* SKSource::getTalker() const {
  if (_input == SKSourceInputNMEA0183_1 || _input == SKSourceInputNMEA0183_2) {
    return _info.nmea.talker;
  }
  else {
    return "";
  }
}

const char* SKSource::getSentence() const {
  if (_input == SKSourceInputNMEA0183_1 || _input == SKSourceInputNMEA0183_2) {
    return _info.nmea.sentence;
  }
  else {
    return "";
  }
}

uint32_t SKSource::getPGN() const {
  if (_input == SKSourceInputNMEA2000) {
    return _info.nmea2000.pgn;
  }
  else {
    return 0;
  }
}

unsigned char SKSource::getPriority() const {
  if (_input == SKSourceInputNMEA2000) {
    return _info.nmea2000.priority;
  }
  else {
    return 0;
  }
}

unsigned char SKSource::getSourceAddress() const {
  if (_input == SKSourceInputNMEA2000) {
    return _info.nmea2000.sourceAddress;
  }
  else {
    return 0;
  }
}
