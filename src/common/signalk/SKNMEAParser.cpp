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

#include <math.h>
#include "common/nmea/NMEASentenceReader.h"
#include "SKUnits.h"
#include "SKNMEAParser.h"

SKNMEAParser::~SKNMEAParser() {
  if (_sku) {
    delete(_sku);
  }
}

const SKUpdate& SKNMEAParser::parse(const SKSourceInput& input, const String& sentence, const SKTime& time) {
  if (_sku) {
    delete(_sku);
  }

  NMEASentenceReader reader = NMEASentenceReader(sentence);

  if (!reader.isValid()) {
    return _invalidSku;
  }

  if (reader.getSentenceCode() == "RMC") {
    return parseRMC(input, reader, time);
  }

  return _invalidSku;
}

const SKUpdate& SKNMEAParser::parseRMC(const SKSourceInput& input, NMEASentenceReader& reader, const SKTime& time) {
  // We first need to make sure the data is valid.
  if (reader.getFieldAsChar(2) != 'A') {
    return _invalidSku;
  }

  SKUpdateStatic<3>* rmc = new SKUpdateStatic<3>();
  rmc->setTimestamp(time);

  SKSource source = SKSource::sourceForNMEA0183(input, reader.getTalkerId(), reader.getSentenceCode());
  rmc->setSource(source);

  double latitude = reader.getFieldAsDouble(3);
  double longitude = reader.getFieldAsDouble(5);
  double sog = SKKnotToMs(reader.getFieldAsDouble(7));
  double cog = SKDegToRad(reader.getFieldAsDouble(8));

  if (!isnan(latitude) && !isnan(longitude)) {
    if (reader.getFieldAsChar(4) == 'S') {
      latitude /= -100;
    } else {
      latitude /= 100;
    }
    if (reader.getFieldAsChar(6) == 'W') {
      longitude /= -100;
    } else {
      longitude /= 100;
    }

    rmc->setValue(SKPathNavigationPosition, SKTypePosition(latitude, longitude, 0));
  }
  if (!isnan(sog)) {
    rmc->setValue(SKPathNavigationSpeedOverGround, sog);
  }
  if (!isnan(cog)) {
    rmc->setValue(SKPathNavigationCourseOverGroundTrue, cog);
  }
  _sku = rmc;
  return *_sku;
}

