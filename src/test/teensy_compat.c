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

/* Very quick and dirty implementation of some functions that are included in
 * GCC for AVR and ARM but not x86.
 * This is so that we can compile parts of the teensy framework when building
 * the tests.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <time.h>

// So that millis() work
uint32_t millis() {
  return 42;
}

// Required by NMEA2000 - Must be defined by application
void delay(uint32_t ms) {
  struct timespec sleeptime;
  sleeptime.tv_sec = 0;
  sleeptime.tv_nsec = ms * 1000 * 1000;
  nanosleep(&sleeptime, NULL);
}

char * ultoa(unsigned int val, char *buf, int radix) {
  if (radix == 8) {
    sprintf(buf, "%o", val);
  }
  else if (radix == 10) {
    sprintf(buf, "%u", val);
  }
  else if (radix == 16) {
    sprintf(buf, "%x", val);
  }
  return buf;
}

char * ltoa(int val, char *buf, int radix) {
  if (radix == 8) {
    sprintf(buf, "%o", val);
  }
  else if (radix == 10) {
    sprintf(buf, "%u", val);
  }
  else if (radix == 16) {
    sprintf(buf, "%x", val);
  }
  return buf;
}

char *dtostrf(float val, int width, unsigned int precision, char *buf) {
  char format[10];
  snprintf(format, sizeof(format), "%%%i.%if", width, precision);

  sprintf(buf, format, val);
  return buf;
}

#ifdef __cplusplus
}
#endif

