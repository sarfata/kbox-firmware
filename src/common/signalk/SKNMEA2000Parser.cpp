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

#include <N2kMessages.h>
#include <KBoxLogging.h>
#include "SKNMEA2000Parser.h"
#include "SKUnits.h"

SKNMEA2000Parser::~SKNMEA2000Parser() {
  if (_sku) {
    delete(_sku);
  }
}

const SKUpdate& SKNMEA2000Parser::parse(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  if (_sku) {
    delete(_sku);
  }

  switch (msg.PGN) {
    /*
    case 126992L: // System Time / Date
        return parse126992(input, msg, timestamp);
      break;
    */
    case 127245L: // Rudder
        return parse127245(input, msg, timestamp);
      break;
    case 127250L: // Vessel Heading
        return parse127250(input, msg, timestamp);
      break;
    case 128259L: // Boat speed
        return parse128259(input, msg, timestamp);
      break;
    case 128267L: // Water depth
        return parse128267(input, msg, timestamp);
      break;
      /*
    case 129026L: // COG SOG rapid
        return parse129026(input, msg, timestamp);
      break;
      */
    case 130306L: // Wind Speed
        return parse130306(input, msg, timestamp);
      break;

    //case 126993: // Heartbeat
    //case 127251: // Rate of Turn
    //case 127257: // Attitude
    //case 127488: // Engine parameters rapid
    //case 127493: // Transmission parameters: dynamic
    //case 127501: // Binary status report
    //case 127505: // Fluid level
    //case 127508: // Battery Status
    //case 127513: // Battery Configuration Status
    //case 129025: // Lat/lon rapid
    //case 129283: // Cross Track Error
    //case 130310: // Outside Environmental parameters
    //case 130311: // Environmental parameters
    //case 130312: // Temperature
    //case 130314: // Pressure
    //case 130316: // Temperature extended range
    default:
      DEBUG("No known conversion for PGN %i", msg.PGN);
      return _invalidSku;
  } // end switch msg.PGN
}
// *****************************************************************************
//  PGN 128259  Boat speed
//  swrt --> tN2kSpeedWaterReferenceType:
//                N2kSWRT_Paddle_wheel=0,
//                N2kSWRT_Pitot_tube=1,
//                N2kSWRT_Doppler_log=2,
//                N2kSWRT_Ultra_Sound=3,
//                N2kSWRT_Electro_magnetic=4
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse128259(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  double waterSpeed;
  double groundSpeed;
  tN2kSpeedWaterReferenceType swrt;

  if (ParseN2kBoatSpeed(msg, sid, waterSpeed, groundSpeed, swrt)) {
    SKUpdateStatic<2> *update = new SKUpdateStatic<2>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);

    if (!N2kIsNA(waterSpeed)) {
      // Vessel speed through the water
      // -> Units: m/s (Meters per second)
      update->setNavigationSpeedThroughWater(waterSpeed);
    }
    if (!N2kIsNA(groundSpeed)) {
      update->setNavigationSpeedOverGround(groundSpeed);
    }

    _sku = update;
    return *_sku;
  }
  else {
    DEBUG("Unable to parse N2kMsg with PGN %i", msg.PGN);
    return _invalidSku;
  }
}
// ****************************************************************************
//  PGN 128267  Water depth
// ****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse128267(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  double depthBelowTransducer = N2kDoubleNA;
  double offset = N2kDoubleNA;

  if (ParseN2kWaterDepth(msg, sid, depthBelowTransducer, offset)) {
    if (!N2kIsNA(depthBelowTransducer)) {
      SKUpdateStatic<3> *update = new SKUpdateStatic<3>();
      update->setTimestamp(timestamp);

      SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
      update->setSource(source);

      update->setEnvironmentDepthBelowTransducer(depthBelowTransducer);

      if (N2kIsNA(offset)) {
        // When no offset then offset should be Zero
        offset = 0;
      }
      // When offset is negative, it's the distance between transducer and keel
      if (offset < 0) {
        update->setEnvironmentDepthTransducerToKeel(offset * -1);
        update->setEnvironmentDepthBelowKeel(depthBelowTransducer + offset);
      }
      else if (offset >= 0) {
        update->setEnvironmentDepthSurfaceToTransducer(offset);
        update->setEnvironmentDepthBelowSurface(depthBelowTransducer + offset);
      }

      _sku = update;
      return *_sku;
    }
  }
  DEBUG("Unable to parse N2kMsg with PGN %i", msg.PGN);
  return _invalidSku;
}

// *****************************************************************************
//   PGN 130306 W I N D
//    tN2kWindReference:
//      N2kWind_True_North=0,
//      N2kWind_Magnetic=1,
//      N2kWind_Apparent=2,
//      N2kWind_True_boat=3
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse130306(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  double windSpeed;     // m/s
  double windAngle;     // in Rad
  tN2kWindReference windReference;

  if (ParseN2kPGN130306(msg,sid,windSpeed,windAngle,windReference)) {
    SKUpdateStatic<2> *update = new SKUpdateStatic<2>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);

    if (!N2kIsNA(windAngle) && !N2kIsNA(windSpeed)) {
      if (windReference == N2kWind_Apprent) {
        update->setEnvironmentWindSpeedApparent(windSpeed);
        if (windAngle >M_PI) windAngle *= -1;
        update->setEnvironmentWindAngleApparent(windAngle);
      }
    }
    // We do not take True Wind Speed as we do not know how the system has calculated it!!
    // Some instruments take SOG/COG then True Wind => Ground Wind

    _sku = update;
    return *_sku;
  }
  else {
    DEBUG("Unable to parse NMEA2000 with PGN %i", msg.PGN);
    return _invalidSku;
  }
}

//  ***********************************************
//   PGN 127245 Rudder
//        →  sid
//        →  RudderPosition [rad]
// *  ********************************************** */
const SKUpdate& SKNMEA2000Parser::parse127245(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char instance;
  tN2kRudderDirectionOrder rudderDirectionOrder;
  double rudderPosition;
  double angleOrder;

  if (ParseN2kRudder(msg,rudderPosition,instance,rudderDirectionOrder,angleOrder)) {
    SKUpdateStatic<1> *update = new SKUpdateStatic<1>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);

    if (!N2kIsNA(rudderPosition) && (rudderPosition < M_PI) && (rudderPosition > M_PI)) {
      // -> Current rudder angle, +ve is rudder to Starboard
      update->setSteeringRudderAngle(rudderPosition);
    }

    _sku = update;
    return *_sku;
  }
  else {
    DEBUG("Unable to parse NMEA2000 with PGN %i", msg.PGN);
    return _invalidSku;
  }
}
// *****************************************************************************
//    PGN 127250 VESSEL HEADING RAPID
//    Heading sensor value with a flag for True or Magnetic.
//    Heading               Heading in radians
//      - Deviation         Magnetic deviation in radians. Use N2kDoubleNA for undefined value.
//      - Variation         Magnetic variation in radians. Use N2kDoubleNA for undefined value.
//      tN2kHeadingReference
//        N2khr_true=0,
//        N2khr_magnetic=1
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse127250(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  tN2kHeadingReference headingReference;
  double heading = N2kDoubleNA;
  double deviation = N2kDoubleNA;
  double variation = N2kDoubleNA;

  if (ParseN2kHeading(msg,sid,heading,deviation,variation,headingReference)) {
    if (!N2kIsNA(heading) && heading >= 0 && heading <= M_PI_2) {
      if (headingReference == N2khr_magnetic) {
        //TODO put 3 when updated to deviation
        SKUpdateStatic<2> *update = new SKUpdateStatic<2>();
        update->setTimestamp(timestamp);

        SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
        update->setSource(source);
        update->setNavigationHeadingMagnetic(heading);
        if (!N2kIsNA(variation))
          update->setNavigationMagneticVariation(variation);
          /* coming when Signal K adds deviation
          if (!N2kIsNA(deviation))
            update->setNavigationMagneticDeviation(deviation);
          */
        _sku = update;
        return *_sku;
      }

      if (headingReference == N2khr_true) {
        SKUpdateStatic<1> *update = new SKUpdateStatic<1>();
        update->setTimestamp(timestamp);

        SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
        update->setSource(source);
        update->setNavigationHeadingTrue(heading);
        _sku = update;
        return *_sku;
      }
    }
  }
  DEBUG("Unable to parse NMEA2000 with PGN %i", msg.PGN);
  return _invalidSku;
}
