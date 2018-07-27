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

#include "SKNMEA2000Converter.h"

#include <N2kMsg.h>
#include <N2kMessages.h>
#include "SKUpdate.h"
#include "SKValue.h"
#include "SKUnits.h"

void SKNMEA2000Converter::convert(const SKUpdate& update, SKNMEA2000Output& out) {
  // Trigger a call of visitSKElectricalBatteriesVoltage for every key with that path
  // (there can be more than one and we do not know how they are called)
  _currentOutput = &out;
  update.accept(*this, SKPathElectricalBatteriesVoltage);

  if (update.hasEnvironmentOutsidePressure()) {
    tN2kMsg msg;
    // PGN 130310 seems to be better supported
    SetN2kOutsideEnvironmentalParameters(msg, 0, N2kDoubleNA, N2kDoubleNA, update.getEnvironmentOutsidePressure());
    out.write(msg);
    // PGN 130314 is more specific and more precise (.1) to pressure but not supported by all displays (ie: Raymarine i70)
    SetN2kPressure(msg, /* sid */ 0, /* source */ 0, N2kps_Atmospheric, update.getEnvironmentOutsidePressure());
    out.write(msg);
  }

  if (update.hasEnvironmentOutsideTemperature()) {
    tN2kMsg msg;
    // PGN 130310 seems to be better supported
    SetN2kOutsideEnvironmentalParameters(msg, 0, N2kDoubleNA, update.getEnvironmentOutsideTemperature(), N2kDoubleNA);
    out.write(msg);
  }

  if (update.hasNavigationAttitude()) {
    tN2kMsg msg;
    SKTypeAttitude attitude = update.getNavigationAttitude();
    SetN2kAttitude(msg, 0, attitude.yaw, attitude.pitch, attitude.roll);
    out.write(msg);
  }

  // PGN 129026: Fast COG/SOG
  if (update.hasNavigationSpeedOverGround() && update.hasNavigationCourseOverGroundTrue()) {
    tN2kMsg msg;
    SetN2kPGN129026(msg, (uint8_t)0, N2khr_true,
        update.getNavigationCourseOverGroundTrue(),
        update.getNavigationSpeedOverGround());
    out.write(msg);
  }

  if (update.hasNavigationHeadingMagnetic()) {
    tN2kMsg msg;
    SetN2kMagneticHeading(msg, 0, update.getNavigationHeadingMagnetic());
    out.write(msg);
  }

  if (update.hasEnvironmentWindAngleApparent() && update.hasEnvironmentWindSpeedApparent()) {
    generateWind(out, update.getEnvironmentWindAngleApparent(), update.getEnvironmentWindSpeedApparent(), N2kWind_Apparent);
  }

  if (update.hasEnvironmentWindAngleTrueWater() && update.hasEnvironmentWindSpeedTrue()) {
    generateWind(out, update.getEnvironmentWindAngleTrueWater(), update.getEnvironmentWindSpeedTrue(), N2kWind_True_water);
  }

  if (update.hasEnvironmentWindAngleTrueGround() && update.hasEnvironmentWindSpeedOverGround()) {
    generateWind(out, update.getEnvironmentWindAngleTrueGround(), update.getEnvironmentWindSpeedOverGround(), N2kWind_True_boat);
  }

  if (update.hasEnvironmentWindDirectionMagnetic() && update.hasEnvironmentWindSpeedOverGround()) {
    generateWind(out, update.getEnvironmentWindDirectionMagnetic(), update.getEnvironmentWindSpeedOverGround(), N2kWind_Magnetic);
  }

  if (update.hasEnvironmentWindDirectionTrue() && update.hasEnvironmentWindSpeedOverGround()) {
    generateWind(out, update.getEnvironmentWindDirectionTrue(), update.getEnvironmentWindSpeedOverGround(), N2kWind_True_North);
  }
}

void SKNMEA2000Converter::visitSKElectricalBatteriesVoltage(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  // PGN127508: Battery Status
  // FIXME: The mapping of Battery instance names to ids should be configurable
  unsigned char instance = 255;
  if (p.getIndex() == "engine") {
    instance = 0;
  }
  else if (p.getIndex() == "house") {
    instance = 1;
  }

  tN2kMsg msg;
  SetN2kDCBatStatus(msg, instance, v.getNumberValue());
  _currentOutput->write(msg);
}

void SKNMEA2000Converter::generateWind(SKNMEA2000Output &out, double windAngle, double windSpeed, tN2kWindReference windReference) {
  tN2kMsg msg;

  // We need to normalize wind to a direction because it is stored as an
  // unsigned double.
  SetN2kWindSpeed(msg, 0, windSpeed, SKNormalizeDirection(windAngle), windReference);

  out.write(msg);
}

