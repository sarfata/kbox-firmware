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


