/*
  * Project:  KBox
  * Purpose:  Set and Load Config Settings
  * Author:   (c) Ronnie Zeiller, 2017
  * KBox:     (c) 2016 Thomas Sarlandie thomas@sarlandie.net

  The MIT License

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

#include <IniFile.h>

// ---------------------------------------------------------------------------
// Define different tasks and pages
// if you don't like a task or page just comment it out
// ---------------------------------------------------------------------------
#define IMU_MONITOR_PAGE

// ---------------------------------------------------------------------------
//   BAUD RATES ON KBOX SERIALS NMEA1 & 2
// ---------------------------------------------------------------------------
// renamed to be ready for other formats
// [serialOut]
extern uint32_t cfCOM_1_baud;
extern uint32_t cfCOM_2_baud;

// ---------------------------------------------------------------------------
//           Use of internal Sensors
// ---------------------------------------------------------------------------
// [sensors]
// TODO: better way to do??!!
extern bool cfUseHdgFromBusIfExists;      // Get Heading from NMEA2000?
extern bool cfUseHdgFromIMUSensor;        // Get Heading from internal IMU-Sensor?
extern bool cfUseHeelPitchFromIMUSensor;  // Get Heel/Pitch from internal IMU-Sensor?
extern int8_t cfIMU_MIN_CAL;              // Minimum Calibration to take HDG from internal sensor

enum KBoxOrientation {
    MOUNTED_ON_PORT_HULL,
    MOUNTED_ON_STB_HULL,
    LAYING_READ_DIR_TO_BOW
};
extern KBoxOrientation cfKBoxOrientation;

// ---------------------------------------------------------------------------
//           Interval Services
// ---------------------------------------------------------------------------
extern int16_t cfIMUServiceInterval;
extern int16_t cfBaroServiceInterval;
extern int16_t cfAdcServiceInterval;

// ---------------------------------------------------------------------------
//              NMEA 2000 Settings
// ---------------------------------------------------------------------------
// [NMEA2000]
extern bool cfEnableN2kToNMEA_127245;  // PGN 127245 RUDDER --> for RSA Rudder Sensor Angle
extern bool cfEnableN2kToNMEA_127250;  // PGN 127250 VESSEL HEADING, for TrueHeading

// TODO: build NMEA0183
extern bool cfEnableN2kToNMEA_129026;  // PGN 129026  COG SOG rapid -> for Rapid COG/SOG
extern bool cfEnableN2kToNMEA_130306;  // PGN 130306 WIND SPEED --> VWR Relative Wind Speed and Angle (openCPN)
extern bool cfEnableN2kToNMEA_128267;
extern bool cfEnableN2kToNmea_128259;

// Which PGNs should be written back to NMEA2000 Bus
extern bool cfSendN2k_TrueWind;
extern bool cfSendN2k_HDG_IMU_intern;
extern bool cfSendN2k_Heel_Pitch_IMU_intern;

// ---------------------------------------------------------------------------
//              Battery Measurement Labels
// ---------------------------------------------------------------------------
// TODO: change to more generic names in ADCService and BatteryMonitorPage?
// enum adcNames { engine = 1, house, dc3, kbox-supply }
// Labels for BatteryMonitorPage
// [labels]
extern String cfLabelADC1;
extern String cfLabelADC2;
extern String cfLabelADC3;
extern String cfLabelADC4;

//----------------------------------------------------------------------------
//    Config
//----------------------------------------------------------------------------
class KBoxConfig {
  private:


    const char *filename = "/KBox.ini";

    char buffer[ BUF_LEN ];
    char *getErrorMessage(uint8_t e, bool eol = true);


  public:

      KBoxConfig();

      int LoadKBoxConfig();

      virtual void UpdateSettings();  // called by:  pConfig->UpdateSettings();

};
