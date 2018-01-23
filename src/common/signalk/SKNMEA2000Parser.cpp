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
    case 126992L: // System Time / Date
      return parse126992(input, msg, timestamp);
    case 127245L: // Rudder
      return parse127245(input, msg, timestamp);
    case 127250L: // Vessel Heading
      if (_config.heading_127250_enabled) {
        return parse127250(input, msg, timestamp);
      }
    //case 127251L: // Rate of Turn
    case 127257L: // Attitude Yaw, Pitch, Roll
      if (_config.attitude_127257_enabled) {
        return parse127257(input, msg, timestamp);
      }
    //case 127258L:  // Magnetic Variation
    //    return parse127258(input, msg, timestamp);
    //  break;
    case 128259L: // Boat speed
      return parse128259(input, msg, timestamp);
    case 128267L: // Water depth
      if (_config.depth_128267_enabled) {
        return parse128267(input, msg, timestamp);
      }
    //case 128275L: // Distance Log
    case 129025L: // Position, Rapid Update Lat/Lon
      return parse129025(input, msg, timestamp);
    case 129026L: // COG SOG rapid
      return parse129026(input, msg, timestamp);
    //case 129301L:  // Time to/from Mark
    case 130306L: // Wind Speed
      return parse130306(input, msg, timestamp);

    //case 127488: // Engine parameters rapid
    //case 127493: // Transmission parameters: dynamic
    //case 127501: // Binary status report
    //case 127505: // Fluid level
    //case 127508: // Battery Status
    //case 127513: // Battery Configuration Status
    //case 129283: // Cross Track Error
    //case 130314: // Actual Pressure
    //case 130316: // Temperature extended range
    //case 130577L: // Direction Data
    //case 130578L: // Vessel Speed Components
    default:
      DEBUG("No known conversion for PGN %i", msg.PGN);
      return _invalidSku;
  } // end switch msg.PGN
}

// *****************************************************************************
//  PGN 126992  System Time Date UTC
// *****************************************************************************
//  - SystemDate    Days since 1970-01-01
//  - SystemTime    seconds since midnight
//  - TimeSource    "GPS", "GLONASS", "radio station", "local cesium clock", "local rubidium clock", "local crystal clock"
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse126992(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;          // Sequence ID
  uint16_t systemDate;        // Days since 1970-01-01
  double systemTime;          // seconds since midnight with 2 digits
  tN2kTimeSource timeSource;  // see tN2kTimeSource (z.B.: GPS)

  if (ParseN2kSystemTime(msg,sid,systemDate,systemTime,timeSource) ) {
    /*
    TODO: Signal K for /environment/time + 3 fields
    SKUpdateStatic<2> *update = new SKUpdateStatic<2>();
    update->setTimestamp(timestamp);

    _sku = update;
    return *_sku;
    */
  }

  DEBUG("Unable to parse N2kMsg with PGN %i", msg.PGN);
  return _invalidSku;
}

//  ***********************************************
//   PGN 127245 Rudder
//        →  sid
//        →  RudderPosition [rad]
// *  ********************************************** */
const SKUpdate& SKNMEA2000Parser::parse127245(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char instance;
  tN2kRudderDirectionOrder rudderDirectionOrder;
  double rudderPosition = N2kDoubleNA;
  double angleOrder = N2kDoubleNA;

  if (ParseN2kRudder(msg,rudderPosition,instance,rudderDirectionOrder,angleOrder)) {
    if (!N2kIsNA(rudderPosition)) {
      SKUpdateStatic<1> *update = new SKUpdateStatic<1>();
      update->setTimestamp(timestamp);

      SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
      update->setSource(source);
      // -> Current rudder angle, +ve is rudder to Starboard
      update->setSteeringRudderAngle(rudderPosition);

      _sku = update;
      return *_sku;
    }
  }

  DEBUG("Unable to parse NMEA2000 with PGN %i", msg.PGN);
  return _invalidSku;
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
    if (!N2kIsNA(heading) && heading >= 0 && heading <= 2 * M_PI) {
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

// *****************************************************************************
// PGN 127257 Attitude Yaw, Pitch, Roll
//  - Yaw                   Heading in radians.
//  - Pitch                 Pitch in radians. Positive, when your bow rises.
//  - Roll                  Roll in radians. Positive, when tilted right.
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse127257(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  double yaw   = N2kDoubleNA;
  double pitch = N2kDoubleNA;
  double roll  = N2kDoubleNA;

  if (ParseN2kPGN127257(msg, sid, yaw, pitch, roll)) {
    SKUpdateStatic<1> *update = new SKUpdateStatic<1>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);
    update->setNavigationAttitude(SKTypeAttitude(roll, pitch, yaw));

    _sku = update;
    return *_sku;
  }

  DEBUG("Unable to parse N2kMsg with PGN %i", msg.PGN);
  return _invalidSku;
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
  double waterSpeed = N2kDoubleNA;
  double groundSpeed = N2kDoubleNA;
  tN2kSpeedWaterReferenceType swrt;

  if (ParseN2kBoatSpeed(msg, sid, waterSpeed, groundSpeed, swrt)) {
    SKUpdateStatic<2> *update = new SKUpdateStatic<2>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);

    if (!N2kIsNA(waterSpeed)) {
      update->setNavigationSpeedThroughWater(waterSpeed);
    }

    _sku = update;
    return *_sku;
  }

  DEBUG("Unable to parse N2kMsg with PGN %i", msg.PGN);
  return _invalidSku;
}

// ****************************************************************************
//  PGN 128267  Water depth
//  Water depth relative to the transducer and offset of the measuring transducer.
//  Water depth is either below water surface or below lowest point of vessel.
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

      // When offset is negative, it's the distance between transducer and keel
      if (!N2kIsNA(offset)) {
        if (offset < 0) {
          update->setEnvironmentDepthTransducerToKeel(offset * -1);
          update->setEnvironmentDepthBelowKeel(depthBelowTransducer + offset);
        }
        else if (offset > 0) {
          update->setEnvironmentDepthSurfaceToTransducer(offset);
          update->setEnvironmentDepthBelowSurface(depthBelowTransducer + offset);
        }
      }

      _sku = update;
      return *_sku;
    }
  }

  DEBUG("Unable to parse N2kMsg with PGN %i", msg.PGN);
  return _invalidSku;
}

// *****************************************************************************
//    129025L: // Position, Rapid Update Lat/Lon
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse129025(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  double latitude;
  double longitude;

  if (ParseN2kPositionRapid(msg, latitude, longitude)) {
    SKUpdateStatic<2> *update = new SKUpdateStatic<2>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);
    update->setNavigationPosition(SKTypePosition(latitude, longitude, 0));

    _sku = update;
    return *_sku;
  }

  DEBUG("Unable to parse NMEA2000 with PGN %i", msg.PGN);
  return _invalidSku;
}

// *****************************************************************************
//    129026: // COG SOG rapid
//    1 Sequence ID
//    2 COG Reference
//    3 NMEA Reserved
//    4 Course Over Ground
//    5 Speed Over Ground
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse129026(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  tN2kHeadingReference headingReference;
  double COG;
  double SOG;

  if (ParseN2kCOGSOGRapid(msg,sid,headingReference,COG,SOG)) {
    SKUpdateStatic<2> *update = new SKUpdateStatic<2>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);
    update->setNavigationCourseOverGroundTrue(COG);
    update->setNavigationSpeedOverGround(SOG);

    _sku = update;
    return *_sku;
  }

  DEBUG("Unable to parse NMEA2000 with PGN %i", msg.PGN);
  return _invalidSku;
}

// *****************************************************************************
//   PGN 130306 W I N D
// The boat referenced true wind is given by the vector sum of Apparent wind and vessel's heading and speed though the water.
// The ground referenced true wind is given by the vector sum of Apparent wind and vessel's heading and speed over ground.
// References and calculations : https://www.rocktheboatmarinestereo.com/specs/MSNRX200I.pdf
//    tN2kWindReference:
//      N2kWind_True_North=0    => Ground Wind, calculated using SOG/COG, referenced to true north
//      N2kWind_Magnetic=1      => Ground Wind, calculated by SOG/COG, referenced to magnetic north
//      N2kWind_Apparent=2      => Apparent Wind, measured on boat, relative to vessel centerline
//      N2kWind_True_boat=3     => Ground Wind, calculated using SOG/COG, relative to centerline
//      N2kWind_True_water=4    => Theoretical Wind, calc using Heading/STW, relative to centerline vessel, referenced to water
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse130306(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  double windSpeed = N2kDoubleNA;
  double windAngle = N2kDoubleNA;
  tN2kWindReference windReference;

  if (ParseN2kPGN130306(msg,sid,windSpeed,windAngle,windReference)) {
    SKUpdateStatic<2> *update = new SKUpdateStatic<2>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);

    if (!N2kIsNA(windAngle) && !N2kIsNA(windSpeed)) {
      switch(windReference) {
        case N2kWind_True_North:
          // Ground Wind Speed
          update->setEnvironmentWindSpeedOverGround(windSpeed);
          // Ground Wind Direction
          update->setEnvironmentWindDirectionTrue(windAngle);
        break;
        case N2kWind_Magnetic:
          // Ground Wind Speed
          update->setEnvironmentWindSpeedOverGround(windSpeed);
          // Ground Wind Direction referred to magnetic north
          update->setEnvironmentWindDirectionMagnetic(windAngle);
        break;
        case  N2kWind_Apparent:
          // AWS Apparent Wind Speed
          update->setEnvironmentWindSpeedApparent(windSpeed);
          // AWA pos coming from starboard, neg from port, relative to centerline vessel
          update->setEnvironmentWindAngleApparent(SKNormalizeAngle(windAngle));
        break;
        case N2kWind_True_boat:
          // Ground Wind
          update->setEnvironmentWindSpeedOverGround(windSpeed);
          // Ground Wind +/- starboard/port
          update->setEnvironmentWindAngleTrueGround(SKNormalizeAngle(windAngle));
        break;
        case N2kWind_True_water:
          // TWS (water referred) True "Sailing" Wind
          update->setEnvironmentWindSpeedTrue(windSpeed);
          update->setEnvironmentWindAngleTrueWater(SKNormalizeAngle(windAngle));
        break;
      }
    }

    _sku = update;
    return *_sku;
  }

  DEBUG("Unable to parse NMEA2000 with PGN %i", msg.PGN);
  return _invalidSku;
}

// *****************************************************************************
//    PGN 128000 Nautical Leeway Angle (new 2017)
// https://www.nmea.org/Assets/20170204%20nmea%202000%20leeway%20pgn%20final.pdf
// Upcoming in Timos library:
// void SetN2kPGN128000(tN2kMsg &N2kMsg, unsigned char SID, double Leeway) {
// N2kMsg.Add2ByteDouble(Leeway,0.0001);
// *****************************************************************************
