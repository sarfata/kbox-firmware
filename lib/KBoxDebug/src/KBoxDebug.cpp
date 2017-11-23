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

#include <Arduino.h>
#include <stdarg.h>
//RES_MOD_11_6_17  add drivers for NMEA1_SERIAL
/* Drivers */
#include <drivers/board.h>

#ifndef DebugSerial
#ifdef ESP8266
//#define DebugSerial Serial1
#define DebugSerial Serial
#else
#define DebugSerial Serial
#endif
#endif


void debug_init() {
  // The speed set here has no importance when logging to USB
  // however, when logging via one of the real serial ports, it helps
  // to make this as fast as possible to avoid lag due to serial port
  // congestion.
  DebugSerial.begin(920800);
}

static int strrpos(const char *string, char c) {
  int index = -1;
  for (int i = 0; string[i] != 0; i++) {
    if (string[i] == c) {
      index = i;
    }
  }
  return index;
}

void debug(const char *fname, int lineno, const char *fmt, ... ) {
  int lastSlash = strrpos(fname, '/');
  if (lastSlash > 0) {
    fname = fname + lastSlash + 1;
  }
  //RES_MOD_10_28_17 if NMEA1083 data skip fname and lineno
  //DebugSerial.print(fname);
  //DebugSerial.print(":");
  //DebugSerial.print(lineno);
  //DebugSerial.print(" ");
  char tmp[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(tmp, 128, fmt, args);
  va_end (args);
  DebugSerial.println(tmp);
  NMEA1_SERIAL.write(tmp);
}
