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

