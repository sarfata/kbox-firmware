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
      //if ( cfEnableSystemDateTimeFromPGN_126992 ) {
        return parse126992(input, msg, timestamp);
      //}
      break;
    case 127245L: // Rudder
      //if ( cfEnableN2kToNMEA_127245 ) {
        return parse127245(input, msg, timestamp);
      //}
      break;
    case 127250L: // Vessel Heading
      //if ( cfEnableN2kToNMEA_127250 ) {
        return parse127250(input, msg, timestamp);
      //}
      break;
    */
    case 128259L: // Boat speed
      //if ( cfEnableN2kToNmea_128259 ) {
        return parse128259(input, msg, timestamp);
      //}
      break;
    case 128267L: // Water depth
      //if ( cfEnableN2kToNMEA_128267 ) {
        return parse128267(input, msg, timestamp);
      //}
      break;
    /*
    case 129026L: // COG SOG rapid
      //if ( cfEnableN2kToNMEA_129026 ) {
        return parse129026(input, msg, timestamp);
      //}
      break;
    case 130306L: // Wind Speed
      //if ( cfEnableN2kToNMEA_130306 ) {
        return parse130306(input, msg, timestamp);
      //}
      break;
    */
    //case 126993: /* Heartbeat */
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

// System Time / Date
// TODO: Set system date and time for SD-Card logging file date and
// Offset to millis() for recalculation of true date/time
// TODO: after 42 days millis() overflow, new time sync needed
const SKUpdate& SKNMEA2000Parser::parse126992(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;          // Sequence ID
  uint16_t SystemDate;        // Days since 1970-01-01
  double SystemTime;          // seconds since midnight mit 2 Kommastellen
  tN2kTimeSource TimeSource;  // see tN2kTimeSource (z.B.: GPS)

    if (ParseN2kSystemTime(msg,sid,SystemDate,SystemTime,TimeSource) ) {

      /*
      // static global variables needed....
      if ( SystemDate > 0 && SystemTime > 0 && dateTime::timeStampNeeded ) {
        // time function class
        dt.setTimeStamp( &SystemDate, &SystemTime);
        // the first 20 Seconds try to get date and time from N2k bus
        if ( millis() > 20000 ) dateTime::timeStampNeeded = false;
      }
      */
    } else {
      INFO("Unable to parse N2kMsg with PGN %i", msg.PGN);
    }
  // nothing todo at the moment
  return _invalidSku;
}

//  ***********************************************
//   PGN 127245 Rudder
//        →  sid
//        →  RudderPosition [rad]
// *  ********************************************** */
const SKUpdate& SKNMEA2000Parser::parse127245(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char Instance;
  tN2kRudderDirectionOrder RudderDirectionOrder;
  double RudderPosition;
  double AngleOrder;

  if (ParseN2kRudder(msg,RudderPosition,Instance,RudderDirectionOrder,AngleOrder) ) {
    // do something.....


    //_sku = update;
    //return *_sku;
  }
  else {
    INFO("Unable to parse N2kMsg with PGN %i", msg.PGN);
  }
  // nothing todo at the moment....
  return _invalidSku;
}

// *****************************************************************************
//   PGN 127250 VESSEL HEADING RAPID
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse127250(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  tN2kHeadingReference HeadingReference;
  double Heading = 361; // not valid value
  double Deviation = 0; // NAN would be better!
  double Variation = 0; // NAN would be better!

  if (ParseN2kHeading(msg,sid,Heading,Deviation,Variation,HeadingReference) ) {

    if ( String(HeadingReference) == "magnetic" ) {
      // Deviation and Variation could be sent
      // (Timo's NMEA Simulator is sending Dev and Var at magnetic HDG only)
      Heading += Deviation + Variation;
    }
    // if ( String(HeadingReference) == "true" ) {  }

    SKUpdateStatic<2> *update = new SKUpdateStatic<2>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);

    // update->.....
    // _sku = update;
    // return *_sku;

  } else {
    INFO("Unable to parse N2kMsg with PGN %i", msg.PGN);
  }
  // as long as we do not fill values....
  return _invalidSku;
}

// *****************************************************************************
//  PGN 128259  Boat speed
//   e.g.: 10kn --> WaterReferenced 5.14 m/s, GroundReferenced: 5.14 m/s, swrt 0
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
      update->setNavigationSpeedThroughWater(waterSpeed);
    }
    if (!N2kIsNA(groundSpeed)) {
      update->setNavigationSpeedOverGround(groundSpeed);
    }

    _sku = update;
    return *_sku;
  }
  else {
    INFO("Unable to parse N2kMsg with PGN %i", msg.PGN);
    return _invalidSku;
  }
}

// ****************************************************************************
//  PGN 128267  Water depth
// ****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse128267(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  double depthBelowTransducer;
  double offset;

  if (ParseN2kWaterDepth(msg, sid, depthBelowTransducer, offset)) {
    SKUpdateStatic<3> *update = new SKUpdateStatic<3>();
    update->setTimestamp(timestamp);

    SKSource source = SKSource::sourceForNMEA2000(input, msg.PGN, msg.Priority, msg.Source);
    update->setSource(source);

    update->setEnvironmentDepthBelowTransducer(depthBelowTransducer);

    if (!N2kIsNA(offset)) {
      // When no offset then offset should be Zero
      offset = 0;
    }
    // When offset is negative or 0, it's the distance between transducer and keel
    if (offset <= 0) {
      update->setEnvironmentDepthTransducerToKeel(offset * -1);
      update->setEnvironmentDepthBelowKeel(depthBelowTransducer + offset);
    }
    else if (offset > 0) {
      update->setEnvironmentDepthSurfaceToTransducer(offset);
      update->setEnvironmentDepthBelowSurface(depthBelowTransducer + offset);
    }

    _sku = update;
    return *_sku;
  }
  else {
    INFO("Unable to parse N2kMsg with PGN %i", msg.PGN);
    return _invalidSku;
  }
}

// *****************************************************************************
//   PGN 129026 COG / SOG
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse129026(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  tN2kHeadingReference HeadingReference;
  double COG;
  double SOG;

  if (ParseN2kCOGSOGRapid(msg,sid,HeadingReference,COG,SOG) ) {
    // do something.....


    //_sku = update;
    //return *_sku;

  }
  else {
    INFO("Unable to parse N2kMsg with PGN %i", msg.PGN);
  }
  // nothing todo at the moment....
  return _invalidSku;
}

// *****************************************************************************
//   PGN 130306 W I N D
//    tN2kWindReference:
//      N2kWind_True_North=0,
//      N2kWind_Magnetic=1,
//      N2kWind_Apprent=2,
//      N2kWind_True_boat=3
// *****************************************************************************
const SKUpdate& SKNMEA2000Parser::parse130306(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp) {
  unsigned char sid;
  double WindSpeed;     // Knots
  double WindAngle;     // in Radians 0... 2*pi
  tN2kWindReference WindReference;

  if (ParseN2kPGN130306(msg,sid,WindSpeed,WindAngle,WindReference) ) {
    // do something.....


    //_sku = update;
    //return *_sku;
  }
  else {
    INFO("Unable to parse N2kMsg with PGN %i", msg.PGN);

  }
  // nothing todo at the moment....
  return _invalidSku;
}
