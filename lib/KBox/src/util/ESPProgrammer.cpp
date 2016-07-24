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
#include <stdlib.h>
#include <KBoxDebug.h>
#include "ESPProgrammer.h"

/* Unless a custom DEBUG output has been configured for this build, then we
 * do not want to log anything in the programmer because that would go straight
 * into esptool.py on the computer and mess things up.
 * When building the 'program-esp' environment, debug output is redirected to
 * Serial3 for debugging.
 */
#ifndef DebugSerial
#define DEBUG(...) /* nop */
#endif

ESPProgrammer::ESPProgrammer(Adafruit_NeoPixel &pixels, usb_serial_class &computerSerial, HardwareSerial &espSerial) :
  pixels(pixels), computerSerial(computerSerial), espSerial(espSerial),
  computerConnection(computerSerial, bootloaderMtu), espConnection(espSerial, bootloaderMtu),
  state(Disconnected)
{
  buffer = (uint8_t*)malloc(bootloaderMtu);
}

ESPProgrammer::~ESPProgrammer() {
  free(buffer);
}

void ESPProgrammer::loop() {
  unsigned int newBaud = computerSerial.baud();
  if (newBaud != currentBaudRate) {
    DEBUG("baud rate change detected - now %i (was %i)", newBaud, currentBaudRate);
    currentBaudRate = newBaud;
    if (computerSerial.baud() != 0) {
      state = ByteMode;

      espSerial.begin(currentBaudRate);
    }
  }

  if (isFrameMode()) {
    loopFrameMode();
  }
  else {
    loopByteMode();
  }

  // If we do not receive anything for more than 3 seconds then we are probably done.
  if (state != Disconnected && state != Done && timeSinceLastByte > 1000) {
    DEBUG("programming done!");
    state = Done;
  }

  // Only update 50 times per sec
  static elapsedMillis rgbTimer = 0;
  if (rgbTimer > 1000 / 50) {
    updateColors();
    rgbTimer = 0;
  }
  // Blink led when data is being transmitted
  static elapsedMillis ledTimer = 0;
  if (ledTimer > 100) {
    static bool ledStatus = false;
    if (timeSinceLastByte < 100) {
      ledStatus = !ledStatus;
    }
    else {
      ledStatus = false;
    }
    digitalWrite(led_pin, ledStatus);
    ledTimer = 0;
  }
}

void ESPProgrammer::loopByteMode() {
  // We keep track of DTR and RTS because this is how esptool.py
  // tells us to reboot the module
  bool changed = false;
  if (computerSerial.dtr() != lastDTR) {
    lastDTR = !lastDTR;
    changed = true;
  }
  if (computerSerial.rts() != lastRTS) {
    lastRTS = !lastRTS;
    changed = true;
  }

  if (changed) {
    DEBUG("Time: %lu DTR: %s RTS: %s", millis(), lastDTR ? "T":"F", lastRTS ? "T":"F");
    if (lastDTR && !lastRTS) {
      DEBUG("ESP Reboot requested");
      state = FrameMode;
      esp_reboot_in_flasher();
    }
  }
  else {
    if (computerSerial.available()) {
      int read = Serial.readBytes((char*)buffer, bootloaderMtu);
      espSerial.write(buffer, read);
      timeSinceLastByte = 0;
    }

    if (espSerial.available()) {
      int read = Serial1.readBytes(buffer, bootloaderMtu);
      computerSerial.write(buffer, read);
      timeSinceLastByte = 0;
    }
  }
}

void ESPProgrammer::loopFrameMode() {
  if (computerConnection.available()) {
    size_t len = computerConnection.readFrame(buffer, bootloaderMtu);
    debugFrame("Computer", buffer, len);

    if (len > sizeof(struct ESPFrameHeader)) {
      struct ESPFrameHeader *header = (struct ESPFrameHeader*) buffer;

      if (header->op == ESP_CMD_SYNC) {
        state = FrameModeSync;
        DEBUG("Sync");
      }
      if (header->op == ESP_CMD_FLASH_BEGIN) {
        state = FrameModeFlash;
        DEBUG("Flash begin");
      }
      if (header->op == ESP_CMD_MEM_BEGIN) {
        state = FrameModeMem;
        DEBUG("Mem begin");
      }
      if (header->op == ESP_CMD_MEM_END) {
        // Mem command is used to flash a "stub" of code
        // and then run it. We need to exit frame mode.
        state = ByteMode;
        DEBUG("Mem end");
      }
      if (header->op == ESP_CMD_FLASH_END) {
        state = ByteMode;
        DEBUG("Flash end");
      }
    }
    espConnection.writeFrame(buffer, len);
    timeSinceLastByte = 0;
  }

  if (espConnection.available()) {
    size_t len = espConnection.readFrame(buffer, bootloaderMtu);
    debugFrame("ESP", buffer, len);
    computerConnection.writeFrame(buffer, len);
    timeSinceLastByte = 0;
  }
}

void ESPProgrammer::updateColors() {
  uint32_t stateColor = 0;

  switch (state) {
    case Disconnected:
      stateColor = pixels.Color(0, 0, 0);
      break;
    case ByteMode:
      stateColor = pixels.Color(0, 0x20, 0);
      break;
    case FrameMode:
      stateColor = pixels.Color(0x20, 0x0, 0);
      break;
    case FrameModeSync:
      stateColor = pixels.Color(0x20, 0x20, 0x20);
      break;
    case FrameModeMem:
      stateColor = pixels.Color(0x20, 0x20, 0);
      break;
    case FrameModeFlash:
      stateColor = pixels.Color(0x20, 0x0, 0x20);
      break;
    case Done:
      stateColor = pixels.Color(0, 0, 0x20);
      break;
  }

  // We want the color to change when data is coming rapidly
  static uint8_t wheelIndex = 0;
  if (timeSinceLastByte < 20) {
    wheelIndex++;
  }

  // Intensity of color will be proportial to how recent the last byte received was
  // After 500ms, the color will be 0
  int dim = 0;
  if (timeSinceLastByte < 500) {
    // set the max to 50 because 255 really just blinds me
    const int maxBrightness = 50;
    dim = maxBrightness - maxBrightness * timeSinceLastByte / 500;
  }

  uint32_t dataColor = dimColor(colorWheel(wheelIndex), dim);

  pixels.setPixelColor(0, stateColor);
  pixels.setPixelColor(1, dataColor);
  pixels.show();
}

bool ESPProgrammer::isFrameMode() const {
  if (state == FrameMode || state == FrameModeMem || state == FrameModeSync
       || state == FrameModeFlash) {
    return true;
  }
  else {
    return false;
  }
}

void ESPProgrammer::debugFrame(const char *name, uint8_t *ptr, size_t len) {
  if (len < sizeof(ESPFrameHeader)) {
    DEBUG("%s: Packet is too small with %i bytes", name, len);
    return;
  }

  struct ESPFrameHeader *header = (struct ESPFrameHeader*)ptr;

  if (len != header->size + sizeof(struct ESPFrameHeader)) {
    DEBUG("%s: Packet Len=%i - zero=%x op=%x size=%i checksum=%i -"
        "INVALID DATA LENGTH len should be: %i",
        name, len, header->zero, header->op, header->size, header->checksum,
        header->size + sizeof(struct ESPFrameHeader));
  }
}


uint32_t ESPProgrammer::dimColor(uint32_t color, uint8_t maxBrightness) {
  uint32_t r = (color & 0xff0000) >> 16;
  uint32_t g = (color & 0xff00) >> 8;
  uint32_t b = color & 0xff;

  r = (r * maxBrightness) << 8;
  g = (g * maxBrightness);
  b = (b * maxBrightness) / 0xff;

  return (r & 0xff0000) | (g & 0xff00) | b;
}

uint32_t ESPProgrammer::colorWheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
