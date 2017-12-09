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

#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Define different tasks and pages
// if you don't like a task or page just comment it out
// ---------------------------------------------------------------------------
#define IMU_MONITOR_PAGE

// ---------------------------------------------------------------------------
//           Use of internal Sensors
// ---------------------------------------------------------------------------
// [sensors]
extern bool cfUseHdgFromBusIfExists;      // Get Heading from NMEA2000?
extern bool cfUseHdgFromIMUSensor;        // Get Heading from internal IMU-Sensor?
extern bool cfUseHeelPitchFromBus;				// Get Heel/Pitch from NMEA2000?
extern bool cfUseHeelPitchFromIMUSensor;  // Get Heel/Pitch from internal IMU-Sensor?
extern int8_t cfHdgMinCal;								// Minimum Calibration to take HDG from internal sensor
extern int8_t cfHeelPitchMinCal;          // Minimum Calibration to take Heel & Pitch from internal sensor

enum KBoxOrientation {
    MOUNTED_UPRIGHT_STB_HULL,
    MOUNTED_UPRIGHT_PORT_HULL,
    LAYING_READ_DIR_TO_BOW,
		LAYING_LEFT_TO_BOW
};
extern KBoxOrientation cfKBoxOrientation;

// ---------------------------------------------------------------------------
//           Interval Services
// ---------------------------------------------------------------------------
extern int16_t cfIMUServiceInterval;
extern int16_t cfBaroServiceInterval;
extern int16_t cfAdcServiceInterval;