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
#include <SPI.h>
#include <Encoder.h>
#include <i2c_t3.h>
#include <KBox.h>
#include "util/ESPProgrammer.h"

// Those are included here to force platformio to link to them.
// See issue github.com/platformio/platformio/issues/432 for a better way to do this.
#include <Bounce.h>

#include "util/SlipStream.h"

#include "ClockPage.h"

ILI9341_t3 *display;
SlipStream slip(Serial, 4096);

void setup() {
  // Reinitialize debug here because in some configurations
  // (like logging to nmea2 output), the kbox setup might have messed
  // up the debug configuration.
  DEBUG_INIT();
  DEBUG("setup done");

  analogWrite(display_backlight, BacklightIntensityMax);

  display = new ILI9341_t3(display_cs, display_dc, 255 /* rst unused */, display_mosi, display_sck, display_miso);

  display->begin();
  display->fillScreen(ILI9341_BLUE);
  display->setRotation(display_rotation);
}

typedef enum {
  CommandSync = 0,
  CommandReset = 1,
  CommandDrawPixels = 2,
  CommandAck = 3,
  CommandCount
} CommandEnum;

struct CommandHeader {
  CommandEnum command;    /* What command to execute */
  uint16_t len;           /* Length of the data array. Can be 0. */
};

struct CommandDrawPixels {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint16_t pixels[];
};

struct Command {
  struct CommandHeader h;
  union {
    uint8_t data[];
    struct CommandDrawPixels cdp;
  };
};

void ackCommand(Command *cmd) {
  CommandHeader ack;
  ack.command = CommandAck;
  ack.len = 0;

  slip.writeFrame((uint8_t*)&ack, sizeof(CommandHeader));
}

void handleCommand(Command *cmd) {
  switch (cmd->h.command) {
    case CommandSync:
      ackCommand(cmd);
      break;
    case CommandReset:
      display->fillScreen(ILI9341_BLACK);
      ackCommand(cmd);
      break;
    case CommandDrawPixels:
      {
        for (int y = 0; y < cmd->cdp.height; y++) {
          for (int x = 0; x < cmd->cdp.width; x++) {
            display->drawPixel(cmd->cdp.x + x, cmd->cdp.y + y, 
                cmd->cdp.pixels[x + y * cmd->cdp.width]);
          }
        }
        ackCommand(cmd);
      }
      break;
    default:
      DEBUG("Unknown command.");
  };
}

bool validateCommand(Command *cmd, size_t len) {
  if (len < sizeof(CommandHeader)) {
    DEBUG("Not enough bytes received for command - len=%i but sizeof(Command)=%i", len, sizeof(Command));
    return false;
  }
  if (len != cmd->h.len + sizeof(CommandHeader)) {
    DEBUG("Length do not match! cmd->h.len=%i but len+sizeof(Command)=%i",
        cmd->h.len, len+sizeof(Command));
    return false;
  }
  if (cmd->h.command >= CommandCount) {
    DEBUG("Invalid command %i", cmd->h.command);
    return false;
  }
  if (cmd->h.command == CommandReset && cmd->h.len != 0) {
    DEBUG("CMD_RESET should have 0 data.");
    return false;
  }
  if (cmd->h.command == CommandDrawPixels) {
    if (cmd->h.len < sizeof(struct CommandDrawPixels)) {
      DEBUG("CMD_DRAW_PIXELS too small! cmd->h.len=%i sizeof(CommandDrawPixels)=%i",
          cmd->h.len, sizeof(struct CommandDrawPixels));
      return false;
    }
    if (cmd->h.len != sizeof(struct CommandDrawPixels) + cmd->cdp.width * cmd->cdp.height * 2) {
      DEBUG("Invalid length for CommandDrawPixels - cmd->h.len=%i should be %i",
          cmd->h.len, sizeof(struct CommandDrawPixels) + cmd->cdp.width * cmd->cdp.height * 2);
      return false;
    }
  }
  return true;
}

void readCommand() {
  if (slip.available()) {
    Command *cmd;

    size_t len = slip.peekFrame((uint8_t**) &cmd);

    if (validateCommand(cmd, len)) {
      handleCommand(cmd);
    }

    // Discard frame and wait for next one
    slip.readFrame(0, 0);
  }
}

void loop() {
  readCommand();
}
