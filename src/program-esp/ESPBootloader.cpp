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

#include <stdint.h>
#include <KBoxDebug.h>
#include "ESPBootloader.h"

void esp_debug_frame(const char *name, uint8_t *ptr, size_t len) {
  if (len < sizeof(ESPFrameHeader)) {
    DEBUG("%s: Packet is too small with %i bytes", name, len);
    return;
  }

  struct ESPFrameHeader *header = (struct ESPFrameHeader*)ptr;

  if (len != header->size + sizeof(struct ESPFrameHeader)) {
    DEBUG("%s: Packet Len=%i - zero=%x op=%x size=%i checksum=%i - INVALID DATA LENGTH len should be: %i", name, len, header->zero, header->op, header->size, header->checksum, header->size + sizeof(struct ESPFrameHeader));
  }
}

