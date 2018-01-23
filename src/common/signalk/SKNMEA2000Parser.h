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

#include <N2kMsg.h>
#include "SKUpdate.h"
#include "SKUpdateStatic.h"
#include "SKNMEA2000ParserConfig.h"

class SKNMEA2000Parser {
  private:
    const SKNMEA2000ParserConfig &_config;
    SKUpdate *_sku = 0;
    SKUpdateStatic<0> _invalidSku = SKUpdateStatic<0>();

  public:
    SKNMEA2000Parser(const SKNMEA2000ParserConfig &config) : _config(config) {};
    ~SKNMEA2000Parser();

    /**
     * Parse a NMEA2000 @param msg received on @param input and returns a
     * constant reference to a `SKUpdate` instance that will remain valid until
     * the next call to `parse()` (or when you destroy this object).
     * Once `parse()` is called again, or the parser is destroyed, the
     * reference is no longer valid!
     */
    const SKUpdate& parse(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);

  private:
    //  PGN 126992  System Time Date
    const SKUpdate& parse126992(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);
    // PGN 127245 Rudder
    const SKUpdate& parse127245(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);
    // PGN 127250 VESSEL HEADING RAPID
    const SKUpdate& parse127250(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);
    // PGN 127257 Attitude Yaw, Pitch, Roll
    const SKUpdate& parse127257(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);
    // PGN 128259 Boat Speed
    const SKUpdate& parse128259(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);
    // PGN 128267 Water depth
    const SKUpdate& parse128267(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);
    // PGN 129025 Position
    const SKUpdate& parse129025(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);
    // PGN 129026 COG & SOG, Rapid Update
    const SKUpdate& parse129026(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);
    // PGN 130306 Wind Speed
    const SKUpdate& parse130306(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);



    // Magnetic variation
    //const SKUpdate& parse127258(const SKSourceInput& input, const tN2kMsg& msg, const SKTime& timestamp);

};
