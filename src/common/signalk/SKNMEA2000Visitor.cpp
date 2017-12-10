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

#include "SKNMEA2000Visitor.h"

#include <N2kMsg.h>
#include <N2kMessages.h>
#include "SKUpdate.h"
#include "SKValue.h"
#include "SKUnits.h"

SKNMEA2000Visitor::SKNMEA2000Visitor() {
}

SKNMEA2000Visitor::~SKNMEA2000Visitor() {
  flushMessages();
}

void SKNMEA2000Visitor::visitSKElectricalBatteriesVoltage(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  // PGN127508: Battery Status
  // FIXME: The mapping of Battery instance names to ids should be configurable
  unsigned char instance = 255;
  if (p.getIndex() == "engine") {
    instance = 0;
  }
  else if (p.getIndex() == "house") {
    instance = 1;
  }

  tN2kMsg *msg = new tN2kMsg();
  SetN2kDCBatStatus(*msg, instance, v.getNumberValue());
  _messages.add(msg);
}

void SKNMEA2000Visitor::visitSKEnvironmentOutsidePressure(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  tN2kMsg *msg = new tN2kMsg();

  // PGN 130310 seems to be better supported
  SetN2kOutsideEnvironmentalParameters(*msg, 0, N2kDoubleNA, N2kDoubleNA, v.getNumberValue());

  // PGN 130314 is more specific to pressure but not supported by Raymarine i70
  //SetN2kPressure(*msg, /* sid */ 0, /* source */ 0, N2kps_Atmospheric, v.getNumberValue());

  _messages.add(msg);
}


void SKNMEA2000Visitor::generateWind(double windAngle, double windSpeed, tN2kWindReference windReference) {
  tN2kMsg *msg = new tN2kMsg();

  // We need to normalize wind to a direction because it is stored as un
  // unsigned double.
  SetN2kWindSpeed(*msg, 0, windSpeed, SKNormalizeDirection(windAngle), windReference);

  _messages.add(msg);
}

void SKNMEA2000Visitor::visitSKEnvironmentWindAngleApparent(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  // Look if we have an update for speed too
  if (!u.hasEnvironmentWindSpeedApparent()) return;

  generateWind(u.getEnvironmentWindAngleApparent(), u.getEnvironmentWindSpeedApparent(), N2kWind_Apparent);
}

void SKNMEA2000Visitor::visitSKEnvironmentWindAngleTrueWater(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  // Look if we have an update for speed too
  if (!u.hasEnvironmentWindSpeedTrue()) return;

  generateWind(u.getEnvironmentWindAngleTrueWater(), u.getEnvironmentWindSpeedTrue(), N2kWind_True_water);
}

void SKNMEA2000Visitor::visitSKEnvironmentWindAngleTrueGround(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  // Look if we have an update for speed too
  if (!u.hasEnvironmentWindSpeedOverGround()) return;

  generateWind(u.getEnvironmentWindAngleTrueGround(), u.getEnvironmentWindSpeedOverGround(), N2kWind_True_boat);
}

void SKNMEA2000Visitor::visitSKEnvironmentWindDirectionMagnetic(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  // Look if we have an update for speed too
  if (!u.hasEnvironmentWindSpeedOverGround()) return;

  generateWind(u.getEnvironmentWindDirectionMagnetic(), u.getEnvironmentWindSpeedOverGround(), N2kWind_Magnetic);
}

void SKNMEA2000Visitor::visitSKEnvironmentWindDirectionTrue(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  // Look if we have an update for speed too
  if (!u.hasEnvironmentWindSpeedOverGround()) return;

  generateWind(u.getEnvironmentWindDirectionTrue(), u.getEnvironmentWindSpeedOverGround(), N2kWind_True_North);
}

void SKNMEA2000Visitor::visitSKNavigationPosition(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  //TODO
}

void SKNMEA2000Visitor::visitSKNavigationAttitude(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  tN2kMsg *msg = new tN2kMsg();
  SetN2kAttitude(*msg, 0, v.getAttitudeValue().yaw, v.getAttitudeValue().pitch, v.getAttitudeValue().roll);
  _messages.add(msg);
}

void SKNMEA2000Visitor::visitSKNavigationHeadingMagnetic(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  tN2kMsg *msg = new tN2kMsg();
  SetN2kMagneticHeading(*msg, 0, v.getNumberValue());
  _messages.add(msg);
}

void SKNMEA2000Visitor::visitSKNavigationSpeedOverGround(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  // PGN 129026: Fast COG/SOG
  if (u.hasNavigationCourseOverGroundTrue()) {
    tN2kMsg *msg = new tN2kMsg();
    SetN2kPGN129026(*msg, (uint8_t)0, N2khr_true,
        u.getNavigationCourseOverGroundTrue(),
        v.getNumberValue());

    _messages.add(msg);
  }
}

const LinkedList<tN2kMsg*>& SKNMEA2000Visitor::getMessages() const {
  return _messages;
}

void SKNMEA2000Visitor::flushMessages() {
  for (LinkedListIterator<tN2kMsg*> it = _messages.begin(); it != _messages.end(); it++) {
    delete (*it);
  }
  _messages.clear();
}
