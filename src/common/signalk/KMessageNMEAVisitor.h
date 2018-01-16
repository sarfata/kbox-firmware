/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

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

#include "KMessage.h"
#include "config/DataFormatConfig.h"

/**
 * Converts KMessage objects into NMEA equivalents which are added
 * to nmeaContent.
 * Call getNMEAContent() to retrieve a string containing one or more
 * NMEA strings.
 * Call clearNMEAContent() to start fresh again.
 */
class KMessageNMEAVisitor : public KVisitor {
  private:
    String nmeaContent;
    const DataFormatConfig &_config;

  public:
    KMessageNMEAVisitor(const DataFormatConfig &config) : _config(config) {};
    void visit(const NMEASentence& s);
    void visit(const NMEA2000Message &n2km);

    String getNMEAContent() const {
      return nmeaContent;
    };

    void clearNMEAContent() {
      nmeaContent = "";
    };
};
