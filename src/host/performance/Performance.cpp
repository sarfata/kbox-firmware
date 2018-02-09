/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License
  Copyright (c) 2018 Ronnie Zeiller ronnie@zeiller.eu
  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

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
/*
  The Performance class is a service (Task) started in main.cpp
  When there is an update for heel coming, the value will be stored.
  When there is an update for boat speed through water coming we calculate
  first leeway and second correct boat speed with cosine of leeway angle.

  TODO:
  1. extend config to choose from where heel is taken (N2k Bus, internal sensor
      or NMEA0183 sensor on serial input)
  2. read correction file for boatspeed offsets for upright and heeled sailing
  3. extend class for corrections on Apparent Wind and calculations of True Wind
*/

#include <KBoxLogging.h>
#include "host/config/KBoxConfig.h"
#include "common/signalk/SKUpdate.h"
#include "common/signalk/SKUpdateStatic.h"
#include "Performance.h"
#include "common/signalk/SKUnits.h"

Performance::Performance(PerformanceConfig &config, SKHub &skHub) :
    Task("Performance"), _config(config), _hub(skHub) {

  _hub.subscribe(this);
  _leeway_deg = SKDoubleNAN;
  _heel_deg = SKDoubleNAN;
  _bsCorr_kts = SKDoubleNAN;
}

void Performance::updateReceived(const SKUpdate& update) {

  if (_config.enabled &&
      update.getSource().getInput() != SKSourceInputPerformance) {

    // heel should come with around 10Hz, so we should have actual heel values
    // for calculating true boatspeed through water
    if (update.hasNavigationAttitude()){
      _heel_deg = SKRadToDeg(update.getNavigationAttitude().roll);
      //DEBUG("SKUpdate --> Heel: %f °", _heel_deg);
    }

    // On most systems boatspeed will come with 1Hz
    if (update.hasNavigationSpeedThroughWater() &&
        update.getNavigationSpeedThroughWater() > 0 ){

      _bs_kts = SKMsToKnot(update.getNavigationSpeedThroughWater()); // SKUpdate in m/s --> knots
      //DEBUG("SKUpdate --> Boatspeed Through Water: %f kts", _bs_kts);
      calcBoatSpeed( _bs_kts );

      // Write updates
      if (_bsCorr_kts != SKDoubleNAN) {
        SKUpdateStatic<2> updateWrite;
        SKSource source = SKSource::performanceCalc();
        updateWrite.setSource(source);
        updateWrite.setTimestamp(millis());

        updateWrite.setNavigationSpeedThroughWater(SKKnotToMs(_bsCorr_kts));

        // Leeway with more than 20° is probably wrong!
        if (_leeway_deg != SKDoubleNAN && _leeway_deg < 20){
          updateWrite.setNavigationSpeedThroughWater(SKDegToRad(_leeway_deg));
        }

        _hub.publish(updateWrite);

        // set back values, because we need them fresh
        _leeway_deg = SKDoubleNAN;
        _heel_deg = SKDoubleNAN;
        _bsCorr_kts = SKDoubleNAN;
      }
    }
  }
}

void Performance::loop() {}

// ****************************************************************************
// Load correction table from SD-Card and correct boat speed
// ****************************************************************************
double Performance::corrForNonLinearTransducer(double &bs_kts, double &heel_deg) {

  // TODO: read file with correction values for boat speed transducer.
  // If heel is more than _maxForHeelCorrectionBoatSpeed then take
  // heeled correction values

  return bs_kts;
}

// ****************************************************************************
// param boatspeed in knots (converted) from SKUpdate
// ****************************************************************************
void Performance::calcBoatSpeed(double &bs_kts) {

  _bsCorr_kts = corrForNonLinearTransducer(bs_kts, _heel_deg);

  // Calculate Leeway
  if (_bsCorr_kts > 0 &&
      calcLeeway(_bsCorr_kts, _heel_deg, _leeway_deg)) {

    double leeway_rad = SKDegToRad(_leeway_deg);
    // Correct measured speed for Leeway
    _bsCorr_kts = abs(_bsCorr_kts / cos(leeway_rad));

    DEBUG("Boatspeed from Transducer: %.3f kts--> Corrected Boatspeed: %.3f kts", bs_kts, _bsCorr_kts);
  } else {
    _bsCorr_kts = SKDoubleNAN;
  }
}

// ****************************************************************************
// Leeway is an angle of drift due to sidewards wind force
// it is depending of a hull-factor (given in config), actual heel and boat speed
// Leeway angle positiv or negative, depending from where the wind is coming.
// ****************************************************************************
bool Performance::calcLeeway(double &bs_kts, double &heel, double &leeway) {

  double leewayHullFactor = _config.leewayHullFactor / 10.0;
  double lw = 0;
  // DEBUG("kBoxConfig leewayHullFactor: %f", leewayHullFactor);

  lw = leewayHullFactor * heel / (bs_kts * bs_kts);
  DEBUG("Leeway : %f°", lw);

  if (lw < 180 && lw > (-180)){
    leeway = lw;
    return true;
  } else {
    leeway = 0;
    return false;
  }
}
