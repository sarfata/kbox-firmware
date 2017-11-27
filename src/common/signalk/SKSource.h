/*
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

#include <WString.h>

enum SKSourceInput {
  SKSourceInputUnknown,
  SKSourceInputNMEA0183_1,
  SKSourceInputNMEA0183_2,
  SKSourceInputNMEA2000
};

const String skSourceInputLabels[] = {
  "InputUnknown",
  "InputNMEA1",
  "InputNMEA2",
  "InputNMEA2000"
};

class SKSource {
  private:
    SKSourceInput _input;
    union {
      struct {
        char talker[3];
        char sentence[4];
      } nmea;
      struct {
        unsigned char sourceAddress;
        unsigned char priority;
        uint32_t pgn;
      } nmea2000;
    } _info;

  public:
    /**
     * Returns an unknown source. 
     * You should use the global SKSourceUnknown instead of calling this
     * function.
     */
    static SKSource unknownSource();

    /**
     * Returns a source instance for the given NMEA0183 source info.
     */
    static SKSource sourceForNMEA0183(const SKSourceInput input, const String& _talker, const String& _sentence);

    /**
     * Returns a source instance for the given NMEA2000 source info.
     */
    static SKSource sourceForNMEA2000(const SKSourceInput input, const uint32_t pgn, const unsigned char priority, const unsigned char sourceAddress);
    /**
     * Compares two SKSource objects and returns true if they represent the same
     * source.
     */
    bool operator==(const SKSource &other) const;
    bool operator!=(const SKSource &other) const;

    const SKSourceInput& getInput() const;
    const String& getType() const;
    const String& getLabel() const;

    // Accessors for NMEA details
    const char* getTalker() const;
    const char* getSentence() const;

    // Accessors for NMEA2000 details
    uint32_t getPGN() const;
    unsigned char getPriority() const;
    unsigned char getSourceAddress() const;
};

/**
 * Define one global singleton instance of SKSource representing an unknown
 * source.
 */
extern const SKSource SKSourceUnknown;
