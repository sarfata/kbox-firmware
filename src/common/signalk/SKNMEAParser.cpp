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
#include <KBoxLogging.h>
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
    _sku = 0;
  }

  NMEASentenceReader reader = NMEASentenceReader(sentence);

  if (!reader.isValid()) {
    DEBUG("%s: Invalid sentence %s", skSourceInputLabels[input].c_str(), sentence.c_str());
    return _invalidSku;
  }

  if (reader.getSentenceCode() == "RMC") {
    return parseRMC(input, reader, time);
  }
  if (reader.getSentenceCode() == "MWV") {
    return parseMWV(input, reader, time);
  }

  DEBUG("%s: %s%s - Unable to parse sentence", skSourceInputLabels[input].c_str(), reader.getTalkerId().c_str(), reader.getSentenceCode().c_str());
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

  double latitude = reader.getFieldAsLatLon(3);
  double longitude = reader.getFieldAsLatLon(5);
  double sog = SKKnotToMs(reader.getFieldAsDouble(7));
  double cog = SKDegToRad(reader.getFieldAsDouble(8));

  if (!isnan(latitude) && !isnan(longitude)) {
    rmc->setValue(SKPathNavigationPosition, SKTypePosition(latitude,
                                                           longitude,
                                                           SKDoubleNAN));
  }
  if (!isnan(sog)) {
    rmc->setValue(SKPathNavigationSpeedOverGround, sog);
  }
  if (!isnan(cog)) {
    rmc->setValue(SKPathNavigationCourseOverGroundTrue, cog);
  }
  // _sku is deleted every time a new sentence is parsed
  _sku = rmc;
  return *_sku;
}

const SKUpdate& SKNMEAParser::parseMWV(const SKSourceInput& input, NMEASentenceReader& reader, const SKTime& time) {
  if (reader.getFieldAsChar(5) != 'A') {
    return _invalidSku;
  }

  double windSpeed = reader.getFieldAsDouble(3);
  double windAngle = reader.getFieldAsDouble(1);
  bool isApparentWind;

  if (isnan(windSpeed) || isnan(windAngle)) {
    return _invalidSku;
  }

  switch (reader.getFieldAsChar(4)) {
    case 'K':
      windSpeed = SKKmphToMs(windSpeed);
      break;
    case 'M':
      // do nothing. wind speed already in m/s.
      break;
    case 'N':
      windSpeed = SKKnotToMs(windSpeed);
      break;
    case 'S':
      windSpeed = SKStatuteMphToMs(windSpeed);
      break;
    default:
      return _invalidSku;
  }

  if (reader.getFieldAsChar(2) == 'R') {
    isApparentWind = true;
  }
  else if (reader.getFieldAsChar(2) == 'T') {
    isApparentWind = false;
  }
  else {
    return _invalidSku;
  }

  // Wind sensors return a number between 0 and 360 but we want an
  // angle in radian with negative values when wind coming from port
  windAngle = SKNormalizeAngle(SKDegToRad(windAngle));

  SKUpdateStatic<2>* wmv = new SKUpdateStatic<2>();
  wmv->setTimestamp(time);

  SKSource source = SKSource::sourceForNMEA0183(input, reader.getTalkerId(), reader.getSentenceCode());
  wmv->setSource(source);

  if (isApparentWind) {
    wmv->setEnvironmentWindAngleApparent(windAngle);
    wmv->setEnvironmentWindSpeedApparent(windSpeed);
  }
  else {
    // Here we are assuming that if we get a true wind, it is true relative to
    // boat speed in water.  This might be incorrect for some elaborate
    // computers that would take GPS data and provide a true wind over ground.
    // TODO: make this a configuration option
    wmv->setEnvironmentWindAngleTrueWater(windAngle);
    wmv->setEnvironmentWindSpeedTrue(windSpeed);
  }

  // _sku is deleted every time a new sentence is parsed
  _sku = wmv;
  return *_sku;
}
