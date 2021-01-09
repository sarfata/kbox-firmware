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
#include <host/config/SDLoggingConfig.h>
#include "../KBoxTest.h"
#include "host/config/KBoxConfigParser.h"

TEST_CASE("KBoxConfigParser") {
  KBoxConfigParser kboxConfigParser("urn:mrn:kbox:unit-testing");
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
    CHECK( config.wifiConfig.vesselURN == "urn:mrn:kbox:unit-testing" );
    CHECK( config.sdLoggingConfig.enabled == true );
    CHECK( config.sdLoggingConfig.logWithoutTime == false );
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

    CHECK( config.wifiConfig.accessPoint.enabled == true );
    CHECK( config.wifiConfig.accessPoint.ssid == "KBox" );
    CHECK( config.wifiConfig.accessPoint.password == "" );

    CHECK( config.wifiConfig.client.enabled == false );
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

  SECTION("WiFi config") {
    const char *jsonConfig = "{ 'client': "
      "   { 'enabled': true, ssid: 'network', 'password': 'secret' }"
      "}";
    JsonObject &root = jsonBuffer.parseObject(jsonConfig);

    CHECK( root.success() );

    WiFiConfig wiFiConfig;
    kboxConfigParser.parseWiFiConfig(root, wiFiConfig);

    CHECK( wiFiConfig.client.enabled == true );
    CHECK( wiFiConfig.client.ssid == "network" );
    CHECK( wiFiConfig.client.password == "secret" );
  }

  SECTION("WiFi config - with no data") {
    const char *jsonConfig = "{ 'client': "
      "   { 'enabled': false }"
      "}";
    JsonObject &root = jsonBuffer.parseObject(jsonConfig);

    CHECK( root.success() );

    WiFiConfig wiFiConfig;
    wiFiConfig.client.ssid = "Default-SSID";
    wiFiConfig.client.password = "Default-password";

    kboxConfigParser.parseWiFiConfig(root, wiFiConfig);

    CHECK( wiFiConfig.client.enabled == false );
    CHECK( wiFiConfig.client.ssid == "Default-SSID" );
    CHECK( wiFiConfig.client.password == "Default-password" );
  }

  SECTION("WiFi config - MMSI") {
    const char *jsonConfig = "{ 'mmsi': '412345678' }";
    JsonObject &root = jsonBuffer.parseObject(jsonConfig);

    CHECK( root.success() );

    WiFiConfig wiFiConfig;

    kboxConfigParser.parseWiFiConfig(root, wiFiConfig);

    CHECK( wiFiConfig.vesselURN == "urn:mrn:imo:mmsi:412345678" );
  }

  SECTION("SDLoggingConfig") {
    const char *jsonConfig = "{ 'enabled': false, 'logWithoutTime': true }";
    JsonObject &root = jsonBuffer.parseObject(jsonConfig);

    CHECK(root.success());

    SDLoggingConfig sdLoggingConfig;

    kboxConfigParser.parseSDLoggingConfig(root, sdLoggingConfig);

    CHECK(!sdLoggingConfig.enabled);
    CHECK(sdLoggingConfig.logWithoutTime);
  }
}
