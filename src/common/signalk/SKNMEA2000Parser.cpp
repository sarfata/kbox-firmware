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

  // Boat speed
  if (msg.PGN == 128259) {
    return parse128259(input, msg, timestamp);
  }
  // Water Depth
  if (msg.PGN == 128267) {
    return parse128267(input, msg, timestamp);
  }

  DEBUG("No known conversion for PGN %i", msg.PGN);
  return _invalidSku;
}

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

    if (waterSpeed != N2kDoubleNA) {
      update->setNavigateSpeedThroughWater(waterSpeed);
    }
    if (groundSpeed != N2kDoubleNA) {
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
    // When offset is negative, it's the distance between transducer and keel
    if (offset < 0) {
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
