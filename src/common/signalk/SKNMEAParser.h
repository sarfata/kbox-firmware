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

#include "SKUpdate.h"
#include "SKUpdateStatic.h"

class NMEASentenceReader;

/**
 * Parse NMEA sentences into SignalK updates.
 */
class SKNMEAParser {
  private:
    SKUpdate *_sku = 0;
    SKUpdateStatic<0> _invalidSku = SKUpdateStatic<0>();

  public:
    SKNMEAParser() {};
    ~SKNMEAParser();

    /**
     * Parses a NMEA0183 @param sentence received on @param input and returns a
     * constant reference to a `SKUpdate` instance that will be valid until you
     * call `parse()` again and the parser exists.
     * Once `parse()` is called again, or the parser is destroyed, the reference
     * is no longer valid!
     */
    const SKUpdate& parse(const SKSourceInput& input, const String& sentence, const SKTime& timestamp);

  private:
    const SKUpdate& parseRMC(const SKSourceInput& input, NMEASentenceReader& reader, const SKTime& timestamp);
    const SKUpdate& parseMWV(const SKSourceInput& input, NMEASentenceReader& reader, const SKTime& timestamp);
};

