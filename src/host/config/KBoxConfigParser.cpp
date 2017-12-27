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

#include "KBoxConfigParser.h"

#define READ_VALUE_WITH_TYPE(name, type) if (json[#name].is<type>()) { config.name = json[#name].as<type>(); }
#define READ_BOOL_VALUE(name) READ_VALUE_WITH_TYPE(name, bool)
#define READ_INT_VALUE(name) READ_VALUE_WITH_TYPE(name, int)
#define READ_INT_VALUE_WRANGE(name, min, max) if (json[#name].is<int>() && json[#name] > min && json[#name] < max) { config.name = json[#name].as<int>(); }

#define READ_ENUM_VALUE(name, converter) if (json[#name].is<const char *>()) { config.name = converter(json[#name].as<const char*>()); }

void KBoxConfigParser::defaultConfig(KBoxConfig &config) {
  config.serial1Config.baudRate = 38400;
  config.serial1Config.inputMode = SerialModeNMEA;
  config.serial1Config.outputMode = SerialModeDisabled;

  config.serial2Config.baudRate = 4800;
  config.serial2Config.inputMode = SerialModeNMEA;
  config.serial2Config.outputMode = SerialModeDisabled;

  config.imuConfig.enabled = true;            // enable internal IMU sensor
  config.imuConfig.enabledHdg = true;         // true if values taken from internal sensor
  config.imuConfig.enabledHeelPitch = true;   // true if values taken from internal sensor
  config.imuConfig.calHdg = 2;                // hdg valid, if calibration value greater equal
  config.imuConfig.calHeelPitch = 2;          // heel (roll) & pitch valid, if calibration value greater equal
  config.imuConfig.writeN2kHdg = false;       // write heading to N2k bus
  config.imuConfig.writeN2kHeelPitch = false; // write heel & pitch values to N2k bus
  config.imuConfig.writeNMEAHdg = true;       // if NMEA hdm enabled write value from unternal sensor
  config.imuConfig.writeNMEAHeelPitch = true; // if NMEA xdrAttitude enabled, write value from internal sensor
}

void KBoxConfigParser::parseKBoxConfig(const JsonObject &json, KBoxConfig &config) {
  defaultConfig(config);

  parseSerialConfig(json["serial1"], config.serial1Config);
  parseSerialConfig(json["serial2"], config.serial2Config);
  parseIMUConfig(json["imu"], config.imuConfig);
  parseBarometerConfig(json["barometer"], config.barometerConfig);
  parseWiFiConfig(json["wifi"], config.wifiConfig);
}

void KBoxConfigParser::parseIMUConfig(const JsonObject &json, IMUConfig &config) {
  READ_BOOL_VALUE(enabled);
  READ_INT_VALUE_WRANGE(frequency, 1, 100);
  READ_BOOL_VALUE(enabledHdg);
  READ_BOOL_VALUE(enabledHeelPitch);
  READ_INT_VALUE_WRANGE(calHdg, 0, 3);
  READ_INT_VALUE_WRANGE(calHeelPitch, 0, 3);
  READ_BOOL_VALUE(writeN2kHdg);
  READ_BOOL_VALUE(writeN2kHeelPitch);
  READ_BOOL_VALUE(writeNMEAHdg);
  READ_BOOL_VALUE(writeNMEAHeelPitch);
}

void KBoxConfigParser::parseBarometerConfig(const JsonObject &json, BarometerConfig &config){
  READ_BOOL_VALUE(enabled);
  READ_INT_VALUE_WRANGE(frequency, 1, 10);
}

void KBoxConfigParser::parseSerialConfig(const JsonObject &json, SerialConfig &config) {
  if (json == JsonObject::invalid()) {
    return;
  }

  READ_INT_VALUE(baudRate);
  READ_ENUM_VALUE(inputMode, convertSerialMode);
  READ_ENUM_VALUE(outputMode, convertSerialMode);

  parseNMEAConverterConfig(json["nmeaConverter"], config.nmeaConverter);
}

void KBoxConfigParser::parseWiFiConfig(const JsonObject &json, WiFiConfig &config) {
  if (json == JsonObject::invalid()) {
    return;
  }

  READ_BOOL_VALUE(enabled);
  parseNMEAConverterConfig(json["nmeaConverter"], config.nmeaConverter);
}

void KBoxConfigParser::parseNMEAConverterConfig(const JsonObject &json, SKNMEAConverterConfig &config) {
  if (json == JsonObject::invalid()) {
    return;
  }

  READ_BOOL_VALUE(xdrPressure);
  READ_BOOL_VALUE(xdrAttitude);
  READ_BOOL_VALUE(hdm);
  READ_BOOL_VALUE(rsa);
  READ_BOOL_VALUE(mwv);
}


enum SerialMode KBoxConfigParser::convertSerialMode(const String &s) {
  if (s == "disabled") {
    return SerialModeDisabled;
  }
  if (s == "nmea") {
    return SerialModeNMEA;
  }
  return SerialModeDisabled;
}
