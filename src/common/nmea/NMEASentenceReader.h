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

/**
 * This class provides a set of functions to make parsing NMEA sentences easier.
 *
 * All the getFieldAs...() functions have a few important things in common:
 *  - Fields are numbered starting at 1 to match the amazing NMEA reference
 *    produced by Eric S Raymond (http://www.catb.org/gpsd/NMEA.html)
 *  - If you do not pass a field id, the reader will auto-increment.
 */
class NMEASentenceReader {
  private:
    String _sentence;

  public:
    /**
     * Creates a new instance of NMEASentenceReader to parse a NMEASentence.
     */
    NMEASentenceReader(const String &sentence)
      : _sentence(sentence) {
    };

    /**
     * Return true if this is a valid NMEA sentence.
     */
    bool isValid() const;

    /**
     * Return a two characters long string identifying the talker.
     */
    const String getTalkerId() const;

    /**
     * Return a three (at most) characters long identifying the sentence.
     */
    const String getSentenceCode() const;

    /**
     * Return the number of fields in the sentence
     * This only counts data fields, not the prefix and checksum.
     */
    int countFields() const;

    /**
     * Return the value of field i as a double or NaN if the field does not
     * exist, or is empty. If a conversion error occurs (the string is not a
     * valid number), 0 will be returned.
     *
     * If i is omitted, auto-incrementation will be used.
     */
    double getFieldAsDouble(int i = 0) const;

    /**
     * Return the value of field i as a char or '\0` if the field does not
     * exist, is empty or is longer than 1 byte.
     *
     * If i is omitted, auto-incrementation will be used.
     */
    char getFieldAsChar(int i = 0) const;

    /**
     * Return the value of field i as a String or an empty string if the field
     * does not exist or is empty.
     *
     * If i is omitted, auto-incrementation will be used.
     */
    const String getFieldAsString(int i = 0) const;

    /**
     * Return the value of field i and i+1 as an angle parsed as latitude or longitude.
     *
     * See http://www.catb.org/gpsd/NMEA.html#_nmea_encoding_conventions
     *
     * 12302.2323,E is 123 degrees East and 02.2323 minutes.
     *
     * Will return NaN if an error occurs while parsing the field.
     */
    double getFieldAsLatLon(int i = 0) const;
};



