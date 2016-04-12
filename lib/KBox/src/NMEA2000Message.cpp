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

#include "KMessage.h"
#include "util/nmea.h"

/* Some helper functions to generate hex-serialized NMEA messages */
static const char *hex = "0123456789ABCDEF";

static int appendByte(char *s, uint8_t byte) {
  s[0] = hex[byte >> 4];
  s[1] = hex[byte & 0xf];
  return 2;
}

static int append2Bytes(char *s, uint16_t i) {
  appendByte(s, i >> 8);
  appendByte(s + 2, i & 0xff);
  return 4;
}

static int appendWord(char *s, uint32_t i) {
  append2Bytes(s, i >> 16);
  append2Bytes(s + 4, i & 0xffff);
  return 8;
}

String NMEA2000Message::toString() const {
  // $PCDIN,<PGN 6>,<Timestamp 8>,<src 2>,<data>*20
  char *buffer = (char*)malloc(6+1+6+1+8+1+2+1+msg.DataLen*2+1+1+2 + 1);

  char *s = buffer;

  strcpy(buffer, "$PCDIN,");
  buffer += 7;
  buffer += appendByte(buffer, msg.PGN >> 16);
  buffer += append2Bytes(buffer, msg.PGN & 0xffff);
  *buffer++ = ',';
  buffer += appendWord(buffer, millis());
  *buffer++ = ',';
  buffer += appendByte(buffer, msg.Source);
  *buffer++ = ',';

  for (int i = 0; i < msg.DataLen; i++) {
    buffer += appendByte(buffer, msg.Data[i]);
  }

  *buffer++ = '*';
  buffer += appendByte(buffer, nmea_compute_checksum(s));
  *buffer = 0;

  String str(s);
  free(s);
  return str;
}


