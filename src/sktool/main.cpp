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

#include <iostream>
#include <string>
#include <WString.h>
#include <ArduinoJson.h>
#include <Seasmart.h>
#include "common/signalk/SKNMEAParser.h"
#include "common/signalk/SKNMEA2000Parser.h"
#include "common/signalk/SKJSONVisitor.h"

SKNMEA2000ParserConfig config;

SKNMEAParser nmeaParser = SKNMEAParser();
SKNMEA2000Parser nmea2000Parser = SKNMEA2000Parser(config);

const SKUpdate& parseInputLine(std::string line) {
  static const SKUpdateStatic<0> noUpdate = SKUpdateStatic<0>();

  if (line.find("$PCDIN") == 0) {
    tN2kMsg msg;

    uint32_t timestamp;
    if (SeasmartToN2k(line.c_str(), timestamp, msg)) {
      return nmea2000Parser.parse(SKSourceInputNMEA2000, msg, timestamp);
    }
    else {
      std::cerr << "Unable to convert PCDIN message to N2kMsg: " << line << std::endl;
      return noUpdate;
    }
  }
  else {
    return nmeaParser.parse(SKSourceInputNMEA0183_1, String(line.c_str()), SKTime(time(0)));
  }
}

const char *vesselURN = "urn:mrn:kbox:validation-tests";

int main(int argc, char **argv) {
  DynamicJsonBuffer jsonBuffer;
  SKJSONVisitor v = SKJSONVisitor(vesselURN, jsonBuffer);

  for (std::string line; std::getline(std::cin, line); ) {
    const SKUpdate& u = parseInputLine(line);

    std::cerr << "Update contains: " << u.getSize() << " values and uses " << u.getSizeBytes() << " bytes." << std::endl;
    JsonObject &o = v.processUpdate(u);
    std::cout << o << std::endl;
  }
}
