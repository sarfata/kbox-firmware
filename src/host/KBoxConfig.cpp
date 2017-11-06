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
#include "KBoxConfig.h"
#include <KBoxLogging.h>

extern KBoxConfig         *pConfig;

// ---------------------------------------------------------------------------
//   BAUD RATES ON KBOX SERIALS NMEA1 & 2
// ---------------------------------------------------------------------------
// renamed to be ready for other formats
// [serial]
uint32_t cfCOM_1_baud     = 19200;
uint32_t cfCOM_2_baud     = 4800;

// ---------------------------------------------------------------------------
//           Use of internal Sensors
// ---------------------------------------------------------------------------
// [sensors]
// TODO: better way to do??!!
bool cfUseHdgFromBusIfExists     = true;      // Get Heading from NMEA2000?
bool cfUseHdgFromIMUSensor       = false;  // Get Heading from internal IMU-Sensor?
bool cfUseHeelPitchFromIMUSensor = true;  // Get Heel/Pitch from internal IMU-Sensor?
int8_t cfIMU_MIN_CAL             = 2;     // Minimum Calibration to take HDG from internal sensor

KBoxOrientation cfKBoxOrientation = MOUNTED_ON_PORT_HULL;

// ---------------------------------------------------------------------------
//           Intervall Tasks
// ---------------------------------------------------------------------------
int8_t cfIMUServiceInterval   = 100;


// ---------------------------------------------------------------------------
//              NMEA 2000 Settings
// ---------------------------------------------------------------------------
// [NMEA2000]
bool cfEnableN2kToNMEA_127245   = true;  // PGN 127245 RUDDER --> for RSA Rudder Sensor Angle
bool cfEnableN2kToNMEA_127250   = true;  // PGN 127250 VESSEL HEADING, for TrueHeading

// TODO: build NMEA0183
bool cfEnableN2kToNMEA_129026   = true;  // PGN 129026  COG SOG rapid -> for Rapid COG/SOG
bool cfEnableN2kToNMEA_130306   = true;  // PGN 130306 WIND SPEED --> VWR Relative Wind Speed and Angle (openCPN)
bool cfEnableN2kToNMEA_128267   = true;
bool cfEnableN2kToNmea_128259   = true;

// Which PGNs should be written back to NMEA2000 Bus
bool cfSendN2k_TrueWind               = true;
bool cfSendN2k_HDG_IMU_intern         = true;
bool cfSendN2k_Heel_Pitch_IMU_intern  = true;

// ---------------------------------------------------------------------------
//              Battery Measurement Labels
// ---------------------------------------------------------------------------
String cfLabelADC1   = "House Battery";
String cfLabelADC2   = "House Current";
String cfLabelADC3   = "Engine Battery";
String cfLabelADC4   = "Supply Voltage";


//-----------------------------------------------------------------------------
//          KBoxConfig Implementation
//-----------------------------------------------------------------------------

KBoxConfig::KBoxConfig(){

}

//-------------------------------------------------------------------------------
//  Read ini-File
//-------------------------------------------------------------------------------
int KBoxConfig::LoadKBoxConfig() {
  DEBUG("Open KBox.ini on SD-Card");


  IniFile ini( filename, FILE_READ, true ); // Case sensitive file name

  #if defined(__MK66FX1M0__) // Teensy 3.6
    // For Builtin SD-Card in Teensy 3.6
    if (!SD.begin()) {
      DEBUG("SD.begin() failed");
      return ( 0 );
    }
  #else
    // KBox, Teensy 3.2
    if (!SD.begin(sdcard_cs)){
      DEBUG("SD.begin(%i) failed", sdcard_cs);
      return ( 0 );
    }
  #endif

  if ( !ini.open() ) {
    DEBUG("ERROR in open ini-File %s", filename);
    return ( 0 );
  }


  // ini-Value with Debug Message if default value is changed
  bool defaultUseHeelPitchFromIMUSensor = cfUseHeelPitchFromIMUSensor;
  bool found = ini.getValue("sensors", "UseHeelPitchFromIMUSensor", buffer, BUF_LEN , cfUseHeelPitchFromIMUSensor);
  if (found && (defaultUseHeelPitchFromIMUSensor != cfUseHeelPitchFromIMUSensor) ) {
    DEBUG("cfUseHeelPitchFromIMUSensor changed to %s", cfUseHeelPitchFromIMUSensor);
  }
  if ( ini.getError() > 0 )  {
    DEBUG("Could not get the value of 'UseHeelPitchFromIMUSensor' in section [sensors]: %s", getErrorMessage(ini.getError()) );
  }

  // ******************  Begin Demo  original IniFile-Library  *************************
  // ini-File exists
  // Check if the file is valid. This can be used to warn if any lines
  // are longer than the buffer.
  if (!ini.validate(buffer, BUF_LEN )) {
    DEBUG("%s not valid, Error: %s", ini.getFilename(), ini.getError() );
    // Cannot do anything else
    //while (1);
  }

  // Fetch a value from a key which is present
  if (ini.getValue("network", "mac", buffer, BUF_LEN )) {
    DEBUG("Section 'network' has an entry 'mac' with value %s", buffer);
  } else {
    DEBUG("Could not read 'mac' from section 'network', error was %s", getErrorMessage(ini.getError()) );
  }

  // Try fetching a value from a missing key (but section is present)
  if (ini.getValue("network", "nosuchkey", buffer, BUF_LEN )) {
    DEBUG("section 'network' has an entry 'nosuchkey' with value %s", buffer);
  } else {
    DEBUG("Could not read 'nosuchkey' from section 'network', error was %s", getErrorMessage(ini.getError()) );
  }

  // Try fetching a key from a section which is not present
  if (ini.getValue("nosuchsection", "nosuchkey", buffer, BUF_LEN )) {
    DEBUG("section 'nosuchsection' has an entry 'nosuchkey' with value %s", buffer);
  } else {
    DEBUG("Could not read 'nosuchkey' from section 'nosuchsection', error was %s", getErrorMessage(ini.getError()) );
  }

  // ******************  End Demo  *************************

  return ( 1 );
}

//-------------------------------------------------------------------------------
//      called by pConfig->UpdateSettings();
//-------------------------------------------------------------------------------
void KBoxConfig::UpdateSettings() {
    // possible future needs of
}


//-------------------------------------------------------------------------------
//  Returns Messages according to different Errors
//-------------------------------------------------------------------------------
char * KBoxConfig::getErrorMessage(uint8_t e, bool eol) {
  char* er = new char[18];
  switch (e) {
    case IniFile::errorNoError:
      strcpy(er, "no error");
      break;
    case IniFile::errorFileNotFound:
      strcpy(er, "file not found");
      break;
    case IniFile::errorFileNotOpen:
      strcpy(er, "file not open");
      break;
    case IniFile::errorBufferTooSmall:
      strcpy(er, "buffer too small");
      break;
    case IniFile::errorSeekError:
      strcpy(er, "seek error");
      break;
    case IniFile::errorSectionNotFound:
      strcpy(er, "section not found");
      break;
    case IniFile::errorKeyNotFound:
      strcpy(er, "key not found");
      break;
    case IniFile::errorEndOfFile:
      strcpy(er, "end of file");
      break;
    case IniFile::errorUnknownError:
      strcpy(er, "unknown error");
      break;
    default:
      strcpy(er, "unknown error value");
      break;
  }
  return er;
}
