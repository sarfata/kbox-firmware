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

#include <Arduino.h>
#include <KBox.h>
#include <util/SlipStream.h>
#include "ESPBootloader.h"

enum ProgrammerState {
  ByteMode,         // Forward every bytes as they come
  FrameMode,        // Read complete frames and then forward them
  FrameModeSync,    // Sync command detected
  FrameModeMem,     // Mem command detected
  FrameModeFlash    // Flash command detected
};

void updateColors(ProgrammerState state, elapsedMillis &timeSinceLastByte);


KBox kbox;
uint8_t buffer[4096];
SlipStream computerConnection(Serial, 4096);
SlipStream espConnection(Serial1, 4096);
ProgrammerState state;
elapsedMillis timeSinceLastByte;


bool isFrameMode(ProgrammerState state) {
  if (state == ByteMode) {
    return false;
  }
  else {
    return true;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(0);
  Serial1.setTimeout(0);

  kbox.setup();

  state = ByteMode;
  updateColors(state, timeSinceLastByte);

  esp_init();
  esp_reboot_in_program();

  DEBUG_INIT();
  DEBUG("Starting esp program");
  Serial3.setTimeout(0);
}

void loop() {
  // On most ESP, the RTS line is used to reset the module
  // DTR is used to boot in flash - or - run program
  if (Serial.rts() && !isFrameMode(state)) {
    DEBUG("rts asserted - going into frame mode");
    state = FrameMode;
    esp_reboot_in_flasher();
  }

  // When in frame mode we read frame by frame
  // this allows us to detect the end of flash mode and then
  // read byte by byte
  if (isFrameMode(state)) {
    if (computerConnection.available()) {
      size_t len = computerConnection.readFrame(buffer, sizeof(buffer));
      esp_debug_frame("Computer", buffer, len);

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
      size_t len = espConnection.readFrame(buffer, sizeof(buffer));
      esp_debug_frame("ESP", buffer, len);
      computerConnection.writeFrame(buffer, len);
      timeSinceLastByte = 0;
    }
  }
  else {
    if (Serial.available()) {
      int read = Serial.readBytes((char*)buffer, sizeof(buffer));
      Serial1.write(buffer, read);
      timeSinceLastByte = 0;
    }

    if (Serial1.available()) {
      int read = Serial1.readBytes(buffer, sizeof(buffer));
      Serial.write(buffer, read);
      timeSinceLastByte = 0;
    }
  }
  // Only update 50 times per sec
  static elapsedMillis rgbTimer = 0;
  if (rgbTimer > 1000 / 50) {
    updateColors(state, timeSinceLastByte);
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

uint32_t dimColor(uint32_t color, uint8_t maxBrightness) {
  uint32_t r = (color & 0xff0000) >> 16;
  uint32_t g = (color & 0xff00) >> 8;
  uint32_t b = color & 0xff;

  r = (r * maxBrightness) << 8;
  g = (g * maxBrightness);
  b = (b * maxBrightness) / 0xff;

  return (r & 0xff0000) | (g & 0xff00) | b;
  
}

uint32_t Wheel(byte WheelPos) {
  Adafruit_NeoPixel &strip = kbox.getNeopixels();

  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

void updateColors(ProgrammerState state, elapsedMillis &timeSinceLastByte) {
  uint32_t stateColor = 0;
  Adafruit_NeoPixel &strip = kbox.getNeopixels();

  switch (state) {
    case ByteMode:
      stateColor = strip.Color(0, 0x20, 0);
      break;
    case FrameMode:
      stateColor = strip.Color(0x20, 0x0, 0);
      break;
    case FrameModeSync:
      stateColor = strip.Color(0x20, 0x20, 0x20);
      break;
    case FrameModeMem:
      stateColor = strip.Color(0x20, 0x20, 0);
      break;
    case FrameModeFlash:
      stateColor = strip.Color(0x20, 0x0, 0x20);
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

  uint32_t dataColor = dimColor(Wheel(wheelIndex), dim);

  kbox.getNeopixels().setPixelColor(0, stateColor);
  kbox.getNeopixels().setPixelColor(1, dataColor);
  kbox.getNeopixels().show();
}

