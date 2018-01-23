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

#include <ArduinoJson.h>
#include <WString.h>
#include "KBoxConfig.h"

/**
 * Load a JSON file into a KBoxConfig object.
 *
 */
class KBoxConfigParser {
  private:
    SerialMode convertSerialMode(const String &s);
    IMUMounting convertIMUMounting(const String &s);

  public:
    /**
     * Configures default values in given KBoxConfig object.
     */
    void defaultConfig(KBoxConfig &config);

    /**
     * Parse given JsonObject and transfers the new values found to the config
     * object.
     *
     * Values not specific in the JSON will have the default values (same as
     * the one provided by defaultConfig().
     */
    void parseKBoxConfig(const JsonObject &object, KBoxConfig &config);

    void parseIMUConfig(const JsonObject &object, IMUConfig &config);
    void parseBarometerConfig(const JsonObject &json, BarometerConfig &config);
    void parseSerialConfig(const JsonObject &object, SerialConfig &config);
    void parseNMEA2000Config(const JsonObject &object, NMEA2000Config &config);
    void parseWiFiConfig(const JsonObject &json, WiFiConfig &config);
    void parseNMEAConverterConfig(const JsonObject &json, SKNMEAConverterConfig &config);
    void parseNMEA2000ParserConfig(const JsonObject &json, SKNMEA2000ParserConfig &config);
};
