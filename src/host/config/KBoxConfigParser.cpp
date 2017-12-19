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

void KBoxConfigParser::defaultConfig(KBoxConfig &config) {
  config.serial1Config.baudRate = 38400;
  config.serial1Config.inputMode = SerialModeNMEA;
  config.serial1Config.outputMode = SerialModeDisabled;

  config.serial2Config.baudRate = 4800;
  config.serial2Config.inputMode = SerialModeNMEA;
  config.serial2Config.outputMode = SerialModeDisabled;

  config.imuConfig.enabled = true;
}

void KBoxConfigParser::parseKBoxConfig(const JsonObject &json, KBoxConfig &config) {
  defaultConfig(config);

  parseNMEAConfig(json["serial1"], config.serial1Config);
  parseNMEAConfig(json["serial2"], config.serial2Config);
  parseIMUConfig(json["imu"], config.imuConfig);
}

void KBoxConfigParser::parseIMUConfig(const JsonObject &json, IMUConfig &imuConfig) {
  if (json["enabled"].is<bool>()) {
    imuConfig.enabled = json["enabled"].as<bool>();
  }
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

void KBoxConfigParser::parseNMEAConfig(const JsonObject &json, SerialConfig &nmeaConfig) {
  if (json == JsonObject::invalid()) {
    return;
  }

  if (json["baudRate"].is<int>()) {
    nmeaConfig.baudRate = json["baudRate"].as<int>();
  }
  if (json["inputMode"].is<const char*>()) {
    nmeaConfig.inputMode = convertSerialMode(json.get<const char*>("inputMode"));
  }
  if (json["outputMode"].is<const char*>()) {
    nmeaConfig.outputMode = convertSerialMode(json.get<const char*>("outputMode"));
  }
}
