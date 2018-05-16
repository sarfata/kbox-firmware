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

#include "KBoxLoggerStream.h"
#include <cstdio>

static int strrpos(const char *string, char c) {
  int index = -1;
  int i = 0;
  for (i = 0; string[i] != 0; i++) {
    if (string[i] == c) {
      index = i;
    }
  }
  return index;
}

KBoxLoggerStream::KBoxLoggerStream(Stream &s) : _stream(s) {
}

void KBoxLoggerStream::log(enum KBoxLoggingLevel level, const char *fname, int lineno, const char *fmt, va_list fmtargs) {
  static const char *logLevelPrefixes[] = { "D", "I", "E", "WD", "WI", "WE" };

  int lastSlash = strrpos(fname, '/');
  if (lastSlash > 0) {
    fname = fname + lastSlash + 1;
  }
  _stream.print(logLevelPrefixes[level]);
  _stream.print(" ");
  _stream.print(fname);
  _stream.print(":");
  _stream.print(lineno);
  _stream.print(" ");

  char tmp[128];
  vsnprintf(tmp, sizeof(tmp), fmt, fmtargs);

  _stream.println(tmp);
};
