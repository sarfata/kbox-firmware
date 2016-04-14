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
#include "nmea.h"
#include "ctype.h"

bool nmea_is_valid(const char *s) {
  if (s[0] != '$' && s[0] != '!') {
    return false;
  }

  uint8_t csum = nmea_compute_checksum(s);
  int rsum = nmea_read_checksum(s);

  if (rsum != -1) {
    return csum == rsum;
  }
  else {
    return true;
  }
}

uint8_t nmea_compute_checksum(const char *sentence) {
  // Skip the $ at the beginning
  int i = 1;

  int checksum = 0;
  while (sentence[i] != '*') {
    checksum ^= sentence[i];
    i++;
  }
  return checksum;
}

static uint8_t hexCharToInt(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  if (c >= 'A' && c <= 'F') {
    return 10 + (c - 'A');
  }
  if (c >= 'a' && c <= 'f') {
    return 10 + (c - 'a');
  }
  return 0;
}

int nmea_read_checksum(const char *sentence) {
  while (*sentence != '*' && *sentence != 0) {
    sentence++;
  }

  // We have reached the end of the string and there is no checksum.
  if (*sentence == 0 || !isxdigit((int)sentence[1])) {
    return -1;
  }

  sentence++;
  int checksum = hexCharToInt(sentence[0]);
  if (isxdigit((int)sentence[1])) {
    checksum <<= 4;
    checksum += hexCharToInt(sentence[1]);
  }
  return checksum;
}

