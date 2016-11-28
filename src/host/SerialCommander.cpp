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

#include "SerialCommander.h"
#include <stdint.h> /* uintXX_t */
#include <stddef.h> /* size_t */
#include <elapsedMillis.h>
#include <KBoxDebug.h>
#include <ILI9341_t3.h>

typedef enum : uint16_t {
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

struct CommandAck {
  uint16_t ackedCommand;
  uint16_t errorCode;
  uint64_t elapsedUs;
} __attribute__((__packed__));

struct Command {
  struct CommandHeader h;
  union {
    uint8_t data[];
    struct CommandDrawPixels cdp;
    struct CommandAck ack;
  };
};

bool SerialCommander::validateCommand(const Command &cmd, size_t len) {
  if (len < sizeof(CommandHeader)) {
    DEBUG("Not enough bytes received for command - len=%i but sizeof(Command)=%i", len, sizeof(Command));
    return false;
  }
  if (len != cmd.h.len + sizeof(CommandHeader)) {
    DEBUG("Length do not match! cmd.h.len=%i but len+sizeof(Command)=%i",
        cmd.h.len, len+sizeof(Command));
    return false;
  }
  if (cmd.h.command >= CommandCount) {
    DEBUG("Invalid command %i", cmd.h.command);
    return false;
  }
  if (cmd.h.command == CommandReset && cmd.h.len != 0) {
    DEBUG("CMD_RESET should have 0 data.");
    return false;
  }
  if (cmd.h.command == CommandDrawPixels) {
    if (cmd.h.len < sizeof(struct CommandDrawPixels)) {
      DEBUG("CMD_DRAW_PIXELS too small! cmd.h.len=%i sizeof(CommandDrawPixels)=%i",
          cmd.h.len, sizeof(struct CommandDrawPixels));
      return false;
    }
    if (cmd.h.len != sizeof(struct CommandDrawPixels) + cmd.cdp.width * cmd.cdp.height * 2) {
      DEBUG("Invalid length for CommandDrawPixels - cmd.h.len=%i should be %i",
          cmd.h.len, sizeof(struct CommandDrawPixels) + cmd.cdp.width * cmd.cdp.height * 2);
      return false;
    }
  }
  return true;
}

void SerialCommander::handleCommand(const Command &cmd) {
  switch (cmd.h.command) {
    case CommandSync:
      break;
    case CommandReset:
      display->fillScreen(ILI9341_BLACK);
      break;
    case CommandDrawPixels:
      {
        // Copy the actual command into a newly allocated buffer.
        struct CommandDrawPixels *cdp = (struct CommandDrawPixels*) malloc(cmd.h.len);
        if (cdp != 0) {
          memcpy(cdp, cmd.data, cmd.h.len);

          // Push the new buffer to the other thread for processing. 
          // We will block here if the queue is full and wait until messages have been processed.
          chMBPost(draw_commands, (msg_t)cdp, TIME_INFINITE);
          // Other thread will free the memory.
        }
      }
      break;
    case CommandAck:
      /* Ignore. Not expecting any ack. */
      break;
    case CommandCount: /* Include here to avoid warning. */
      break;
  };
}

void SerialCommander::ackCommand(const Command &cmd, uint16_t errorCode, uint64_t elapsedUs) {
  Command ack;
  ack.h.command = CommandAck;
  ack.h.len = sizeof(struct CommandAck);
  ack.ack.ackedCommand = cmd.h.command;
  ack.ack.errorCode = errorCode;
  ack.ack.elapsedUs = elapsedUs;

  slip.writeFrame((uint8_t*)&ack, sizeof(CommandHeader) + ack.h.len);
}


void SerialCommander::loop() {
  while (1) {
    if (slip.available()) {
      Command *cmd;

      size_t len = slip.peekFrame((uint8_t**) &cmd);

      elapsedMicros e;

      if (validateCommand(*cmd, len)) {
        handleCommand(*cmd);
        ackCommand(*cmd, 0x00, e);
      }
      else {
        ackCommand(*cmd, 0xff01, e);
      }

      // Discard frame and wait for next one
      slip.readFrame(0, 0);
    }
  }
}
