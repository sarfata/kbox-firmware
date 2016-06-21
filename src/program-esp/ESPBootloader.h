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

#include <stddef.h>
#include <stdint.h>

// From esptool.py - List of bootloader commands
#define ESP_CMD_FLASH_BEGIN 0x02
#define ESP_CMD_FLASH_END 0x04
#define ESP_CMD_MEM_BEGIN 0x05
#define ESP_CMD_MEM_END 0x06
#define ESP_CMD_SYNC 0x08

// From esptool.py: format of a ESP frame (when in bootloader mode)
struct ESPFrameHeader {
  uint8_t zero;
  uint8_t op;
  uint16_t size;
  uint32_t checksum;
};

void esp_debug_frame(const char *name, uint8_t *ptr, size_t len);
