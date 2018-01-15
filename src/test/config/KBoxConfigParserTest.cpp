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

#include <ArduinoJson.h>
#include "../KBoxTest.h"
#include "host/config/KBoxConfigParser.h"

TEST_CASE("KBoxConfigParser") {
  KBoxConfigParser kboxConfigParser;
  KBoxConfig config;
  StaticJsonBuffer<4096> jsonBuffer;

  SECTION("Default config") {
    kboxConfigParser.defaultConfig(config);

    // check some default values
    CHECK( config.serial1Config.baudRate == 38400 );
    CHECK( config.serial1Config.inputMode == SerialModeNMEA );
    CHECK( config.serial1Config.outputMode == SerialModeNMEA );
    CHECK( config.serial2Config.baudRate == 4800 );
    CHECK( config.serial2Config.outputMode == SerialModeNMEA );
    CHECK( config.nmea2000Config.txEnabled == true );
    CHECK( config.nmea2000Config.rxEnabled == true );
    CHECK( config.wifiConfig.enabled == true );
  }

  SECTION("No input") {
    JsonObject& root = jsonBuffer.parseObject("invalid json string");

    kboxConfigParser.parseKBoxConfig(root, config);

    // check some default values
    CHECK( config.serial1Config.baudRate == 38400 );
    CHECK( config.serial1Config.inputMode == SerialModeNMEA );
    CHECK( config.serial1Config.outputMode == SerialModeNMEA );
    CHECK( config.serial2Config.baudRate == 4800 );

    CHECK( config.imuConfig.enableHdg );
    CHECK( config.imuConfig.enableHeelPitch );
  }

  SECTION("basic config") {
    const char* jsonConfig = "{ 'serial1': { 'inputMode': 'disabled', 'outputMode': 'nmea', 'baudRate': 4800 }, \
                                'serial2': { 'baudRate': 38400 }, \
                                'imu': { 'enabled': false, frequency: 5, 'mounting': 'horizontalLeftSideToBow' }\
                              }";

    JsonObject& root = jsonBuffer.parseObject(jsonConfig);

    CHECK( root.success() );

    kboxConfigParser.parseKBoxConfig(root, config);

    CHECK( config.serial1Config.baudRate == 4800 );
    CHECK( config.serial1Config.inputMode == SerialModeDisabled );
    CHECK( config.serial1Config.outputMode == SerialModeNMEA );

    CHECK( config.serial2Config.baudRate == 38400 );

    CHECK( ! config.imuConfig.enabled );
    CHECK( config.imuConfig.frequency == 5 );
    CHECK( config.imuConfig.mounting ==  horizontalLeftSideToBow );
  }

  SECTION("NMEA Converter Config") {
    const char *jsonConfig = "{ 'xdr': true, 'mwv': false }";

    JsonObject &root = jsonBuffer.parseObject(jsonConfig);

    CHECK( root.success() );

    SKNMEAConverterConfig nmeaConfig;
    kboxConfigParser.parseNMEAConverterConfig(root, nmeaConfig);

    CHECK( nmeaConfig.xdrPressure == true );
    CHECK( nmeaConfig.mwv == false );
  }
}
