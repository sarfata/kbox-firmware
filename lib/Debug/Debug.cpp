/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <Arduino.h>
#include <stdarg.h>

#ifdef ESP8266
//#define DebugSerial Serial1
#define DebugSerial Serial
#else
#define DebugSerial Serial
#endif


void debug_init() {
    DebugSerial.begin(115200);
}

void debug(const char *fname, int lineno, const char *fmt, ... ) {
  DebugSerial.print(fname);
  DebugSerial.print(":");
  DebugSerial.print(lineno);
  DebugSerial.print(" ");
  char tmp[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(tmp, 128, fmt, args);
  va_end (args);
  DebugSerial.println(tmp);
}
