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
  config.serial1Config.outputMode = SerialModeNMEA;

  config.serial2Config.baudRate = 4800;
  config.serial2Config.inputMode = SerialModeNMEA;
  config.serial2Config.outputMode = SerialModeNMEA;
  config.serial2Config.nmeaConverter.xdrAttitude = false;
  config.serial2Config.nmeaConverter.xdrBattery = false;
  config.serial2Config.nmeaConverter.xdrPressure = false;

  config.nmea2000Config.txEnabled = true;
  config.nmea2000Config.rxEnabled = true;

  config.imuConfig.enabled = true;
  config.imuConfig.frequency = 20;
  config.imuConfig.enabled = true;           // enable internal IMU sensor
  config.imuConfig.enableHdg = true;         // true if values taken from internal sensor
  config.imuConfig.enableHeelPitch = true;   // true if values taken from internal sensor
  config.imuConfig.mounting = VerticalStbHull;

  config.barometerConfig.enabled = true;
  config.barometerConfig.frequency = 1;

  config.wifiConfig.enabled = true;

  config.analogSensorConfig.enabled = false;
  config.analogSensorConfig.frequency = 10;
  config.analogSensorConfig.pulsesPerNauticalMile = 20000;
}

void KBoxConfigParser::parseKBoxConfig(const JsonObject &json, KBoxConfig &config) {
  defaultConfig(config);

  parseSerialConfig(json["serial1"], config.serial1Config);
  parseSerialConfig(json["serial2"], config.serial2Config);
  parseIMUConfig(json["imu"], config.imuConfig);
  parseBarometerConfig(json["barometer"], config.barometerConfig);
  parseWiFiConfig(json["wifi"], config.wifiConfig);
  parseNMEA2000Config(json["nmea2000"], config.nmea2000Config);
  parseAnalogSensorConfig(json["analogSensor"], config.analogSensorConfig);
}

void KBoxConfigParser::parseIMUConfig(const JsonObject &json, IMUConfig &config) {
  READ_BOOL_VALUE(enabled);
  READ_INT_VALUE_WRANGE(frequency, 1, 100);
  READ_BOOL_VALUE(enableHdg);
  READ_BOOL_VALUE(enableHeelPitch);
  READ_ENUM_VALUE(mounting, convertIMUMounting);
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

void KBoxConfigParser::parseNMEA2000Config(const JsonObject &json,
                                           NMEA2000Config &config) {
  READ_BOOL_VALUE(rxEnabled);
  READ_BOOL_VALUE(txEnabled);
}

void KBoxConfigParser::parseWiFiConfig(const JsonObject &json, WiFiConfig &config) {
  if (json == JsonObject::invalid()) {
    return;
  }

  READ_BOOL_VALUE(enabled);
  parseNMEAConverterConfig(json["nmeaConverter"], config.nmeaConverter);
}

void KBoxConfigParser::parseAnalogSensorConfig(const JsonObject &json,
                                               AnalogSensorConfig &config) {
  if (json == JsonObject::invalid()) {
    return;
  }

  READ_BOOL_VALUE(enabled);
  READ_INT_VALUE(frequency);
  READ_INT_VALUE_WRANGE(pulsesPerNauticalMile, 1, 100000);
}

void KBoxConfigParser::parseNMEAConverterConfig(const JsonObject &json, SKNMEAConverterConfig &config) {
  if (json == JsonObject::invalid()) {
    return;
  }

  READ_BOOL_VALUE(xdrPressure);
  READ_BOOL_VALUE(xdrAttitude);
  READ_BOOL_VALUE(xdrBattery);
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

enum IMUMounting KBoxConfigParser::convertIMUMounting(const String &s) {
  if (s == "verticalPortHull") {
    return VerticalPortHull;
  }
  if (s == "verticalStarboardHull") {
    return VerticalStbHull;
  }
  if (s == "verticalTopToBow") {
    return VerticalTopToBow;
  }
  if (s == "horizontalLeftSideToBow") {
    return HorizontalLeftSideToBow;
  }
  // default
  return VerticalPortHull;
}
