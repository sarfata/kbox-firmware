/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

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

#include "SKUnits.h"
#include "common/nmea/NMEASentenceBuilder.h"

#include "SKNMEAVisitor.h"

// debug only
#include <KBoxLogging.h>

void SKNMEAVisitor::visitSKElectricalBatteriesVoltage(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  NMEASentenceBuilder sb("II", "XDR", 4);
  sb.setField(1, "V");
  sb.setField(2, v.getNumberValue(), 2);
  sb.setField(3, "V");
  sb.setField(4, p.getIndex());
  _sentences.add(sb.toNMEA() + "\r\n");
}

void SKNMEAVisitor::visitSKEnvironmentOutsidePressure(const SKUpdate& u, const SKPath &p, const SKValue &v) {
  // XDR is not a very well defined sentence. Can be used for lots of things
  // apparently but that is better than nothing.
  NMEASentenceBuilder sb("II", "XDR", 4);
  sb.setField(1, "P");
  sb.setField(2, v.getNumberValue(), 5);
  sb.setField(3, "B");
  sb.setField(4, "Barometer");
  _sentences.add(sb.toNMEA() + "\r\n");
}

void SKNMEAVisitor::visitSKNavigationAttitude(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  // Same comment on XDR. It's not very well standardized but at least its a way
  // to share the data.
  NMEASentenceBuilder sb( "II", "XDR", 8);
  sb.setField(1, "A");
  sb.setField(2, SKRadToDeg(v.getAttitudeValue().pitch), 1);
  sb.setField(3, "D");
  sb.setField(4, "PTCH");
  sb.setField(5, "A");
  sb.setField(6, SKRadToDeg(v.getAttitudeValue().roll), 1);
  sb.setField(7, "D");
  sb.setField(8, "ROLL");

  _sentences.add(sb.toNMEA() + "\r\n");
};

void SKNMEAVisitor::visitSKNavigationHeadingMagnetic(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  NMEASentenceBuilder sb("II", "HDM", 2);
  sb.setField(1, SKRadToDeg(v.getNumberValue()), 1);
  sb.setField(2, "M");

  _sentences.add(sb.toNMEA() + "\r\n");
};

// ***********************  Wind Speed and Angle  ************************
//  Talker ID: WI Weather Instruments
//             VW Velocity Sensor, Mechanical?
//  also seen: IIVWR
//
//  NMEA0183  MWV Wind Speed and Angle  --> recommanded
//              1  2  3  4
//              |  |  |  |
//      $--MWV,x.x,a,x.x,a*hh
//  1) Wind Angle, 0 to 360 degrees
//  2) Reference, R = Relative, T = True
//  3) Wind Speed
//  4) Wind Speed Units: K = km/s, M = m/s, N = Knots
//  5) Status, A = Data Valid
//
//      VWR Relative Wind Speed and Angle  --> deprecated
//              1  2  3  4  5  6  7  8
//              |  |  |  |  |  |  |  |
//      $--VWR,x.x,a,x.x,N,x.x,M,x.x,K*hh
//  1) Wind direction magnitude in degrees
//  2) Wind direction Left/Right of bow
//  3) Speed
//  4) N = Knots
//  5) Speed
//  6) M = Meters Per Second
//  7) Speed
//  8) K = Kilometers Per Hour
// ************************************************************************
void SKNMEAVisitor::visitSKEnvironmentWindAngleApparent(const SKUpdate &u, const SKPath &p, const SKValue &v) {

  float windAngle;
  float windSpeed;
  windAngle = v.getNumberValue();

  // Validation
  if ((windAngle < 0)||(windAngle > 2 * M_PI)||(!u.hasEnvironmentWindSpeedApparent())) return;

  windSpeed = u.getEnvironmentWindSpeedTrue();  // in m/s

  NMEASentenceBuilder sb( "II", "MWV", 5);
  sb.setField(1, SKRadToDeg(windAngle), 2 );
  sb.setField(2, "R");
  sb.setField(3, windSpeed, 2 );
  sb.setField(4, "M");
  sb.setField(5, "A");
  _sentences.add(sb.toNMEA() + "\r\n");
}

//  ***********************************************
//    RSA Rudder Sensor Angle
//    Talker-ID: AG - Autopilot general
//    also seen: ERRSA
//    Expedition: IIXDR
//
//            1  2  3  4
//            |  |  |  |
//    $--RSA,x.x,A,x.x,A*hh
//      1) Starboard (or single) rudder sensor, "-" means Turn To Port
//      2) Status, A means data is valid
//      3) Port rudder sensor
//      4) Status, A means data is valid
// *********************************************** */
void SKNMEAVisitor::visitSKSteeringRudderAngle(const SKUpdate &u, const SKPath &p, const SKValue &v) {
  NMEASentenceBuilder sb("II", "RSA", 4);
  sb.setField(1, SKRadToDeg( v.getNumberValue() ),1 );
  sb.setField(2, "A");
  sb.setField(3, "");
  sb.setField(4, "");
  _sentences.add(sb.toNMEA() + "\r\n");
}

//  ***********************************************
//  New NMEA 0183 sentence Leeway
//  https://www.nmea.org/Assets/20170303%20nautical%20leeway%20angle%20measurement%20sentence%20amendment.pdf
//         1  2
//         |  |
//  $--LWY,A,x.x*hh<CR><LF>
//      1) Valid or not A/V
//      2) leeway in degrees and decimal degrees, positiv slipping to starboard
//  ***********************************************
