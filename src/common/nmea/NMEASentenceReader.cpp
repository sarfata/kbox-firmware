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

#include <math.h> // NAN
#include "NMEASentenceReader.h"
#include "nmea.h"

bool NMEASentenceReader::isValid() const {
  return nmea_is_valid(_sentence.c_str());
}

const String NMEASentenceReader::getTalkerId() const {
  return _sentence.substring(1, 3);
}

const String NMEASentenceReader::getSentenceCode() const {
  return _sentence.substring(3, _sentence.indexOf(','));
}

int NMEASentenceReader::countFields() const {
  const char *s = _sentence.c_str();
  int count = 0;

  int i = 0;
  while (s[i] != '\0') {
    if (s[i] == ',') {
      count++;
    }
    i++;
  }
  return count;
}

const String NMEASentenceReader::getFieldAsString(int id) const {
  int pos = 0;
  int index = 0;

  while(index < id) {
    pos = _sentence.indexOf(',', pos + 1);
    if (pos == -1) {
      break;
    }
    index++;
  }
  if (pos == -1) {
    return "";
  }
  else {
    // fields are terminated by another ',' or by '*'
    int end = _sentence.indexOf(',', pos + 1);
    if (end == -1) {
      end = _sentence.indexOf('*', pos + 1);
    }
    if (end == -1) {
      // Something went wrong here :(
      return "";
    }

    return _sentence.substring(pos + 1, end);
  }
}

char NMEASentenceReader::getFieldAsChar(int id) const {
  String f = getFieldAsString(id);
  if (f.length() != 1) {
    return '\0';
  }
  else {
    return f.c_str()[0];
  }
}

double NMEASentenceReader::getFieldAsDouble(int id) const {
  String f = getFieldAsString(id);

  if (f.length() == 0) {
    return NAN;
  }
  return strtod(f.c_str(), 0);
}
