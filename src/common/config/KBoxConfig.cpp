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

#include "KBoxConfig.h"

// ---------------------------------------------------------------------------
//           Use of internal Sensors
// ---------------------------------------------------------------------------
// [sensors]
bool cfUseHdgFromBusIfExists			= true;   // Get Heading from NMEA2000?
bool cfUseHdgFromIMUSensor				= true;		// Get Heading from internal IMU-Sensor?
bool cfUseHeelPitchFromBus				= true;		// Get Heel/Pitch from NMEA2000?
bool cfUseHeelPitchFromIMUSensor	= true;   // Get Heel/Pitch from internal IMU-Sensor?
int8_t cfHdgMinCal								= 2;      // Minimum Calibration to take HDG from internal sensor
int8_t cfHeelPitchMinCal					= 2;      // Minimum Calibration to take Heel & Pitch from internal sensor

// MOUNTED_UPRIGHT_STB_HULL, MOUNTED_UPRIGHT_PORT_HULL, LAYING_READ_DIR_TO_BOW, LAYING_LEFT_TO_BOW
KBoxOrientation cfKBoxOrientation = MOUNTED_UPRIGHT_STB_HULL;

// ---------------------------------------------------------------------------
//           Interval Services
// ---------------------------------------------------------------------------
int16_t cfIMUServiceInterval   = 50;
int16_t cfBaroServiceInterval  = 1000;
int16_t cfAdcServiceInterval   = 1000;
