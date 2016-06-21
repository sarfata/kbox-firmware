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

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <Adafruit_NeoPixel.h>
#include <elapsedMillis.h>
#include "util/SlipStream.h"
#include "drivers/board.h"
#include "drivers/esp8266.h"

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

/** Implements support for programming the ESP from the computer.
 */
class ESPProgrammer {
  enum ProgrammerState {
    Disconnected,     // Disconnected
    ByteMode,         // Forward every bytes as they come
    FrameMode,        // Read complete frames and then forward them
    FrameModeSync,    // Sync command detected
    FrameModeMem,     // Mem command detected
    FrameModeFlash    // Flash command detected
  };

  static const int bootloaderMtu = 4096;
  uint8_t *buffer;
  Adafruit_NeoPixel &pixels;

  usb_serial_class &computerSerial;
  HardwareSerial &espSerial;
  SlipStream computerConnection;
  SlipStream espConnection;

  ProgrammerState state;
  elapsedMillis timeSinceLastByte;
  unsigned int currentBaudRate = 0;

  private:
    bool isFrameMode() const;
    void updateColors();
    void loopByteMode();
    void loopFrameMode();
    void debugFrame(const char *name, uint8_t *ptr, size_t len);

    uint32_t colorWheel(byte WheelPos);
    uint32_t dimColor(uint32_t color, uint8_t maxBrightness);

  public:
    ESPProgrammer(Adafruit_NeoPixel &pixels, usb_serial_class &computerSerial, HardwareSerial &espSerial);
    ~ESPProgrammer();

    void loop();
};
