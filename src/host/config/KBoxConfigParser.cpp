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
#include <KBoxLogging.h>

#include <WString.h>

void KBoxConfigParser::defaultConfig(KBoxConfig &config) {
  config.nmea1Config.baudRate = 38400;
  config.nmea1Config.inputEnabled = true;
  config.nmea1Config.outputEnabled = false;

  config.nmea2Config.baudRate = 4800;
  config.nmea2Config.inputEnabled = true;
  config.nmea2Config.outputEnabled = false;

  config.imuConfig.enabled = true;
}

void KBoxConfigParser::parseKBoxConfig(const JsonObject &json, KBoxConfig &config) {
  defaultConfig(config);

  parseNMEAConfig(json["nmea1"], config.nmea1Config, 1);
  parseNMEAConfig(json["nmea2"], config.nmea2Config, 2);
  parseIMUConfig(json["imu"], config.imuConfig);
}

void KBoxConfigParser::parseIMUConfig(const JsonObject &json, IMUConfig &imuConfig) {
  if (json["enabled"].is<bool>()) {
    imuConfig.enabled = json["enabled"].as<bool>();
  }
}

void KBoxConfigParser::parseNMEAConfig(const JsonObject &json, NMEAConfig &nmeaConfig, int nmeaIndex) {
  if (json == JsonObject::invalid()) {
    return;
  }

  if (json["baudRate"].is<int>()) {
    nmeaConfig.baudRate = json["baudRate"].as<int>();
  }
  if (json["inputEnabled"].is<bool>()) {
    nmeaConfig.inputEnabled = json["inputEnabled"].as<bool>();
  }
  if (json["outputEnabled"].is<bool>()) {
    nmeaConfig.outputEnabled = json["outputEnabled"].as<bool>();
  }
}
