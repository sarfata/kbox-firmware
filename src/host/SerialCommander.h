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

#include <Stream.h>
#include <ChibiOS_ARM.h>
#include <stdint.h>
#include "util/SlipStream.h"

class ILI9341_t3;
struct Command;
struct CommandDrawPixels;

class SerialCommander {
  private:
    SlipStream slip;
    /* FIXME: Disgusting workaround to quickly try something. */
    ILI9341_t3 *display;
    mailbox_t *draw_commands;


    bool validateCommand(const Command &cmd, size_t len);
    void handleCommand(const Command &cmd);
    void ackCommand(const Command &cmd, uint16_t errorCode, uint64_t elapsedUs);

  public:
    SerialCommander(Stream &s, size_t mtu, ILI9341_t3 *display, mailbox_t *draw_commands) : slip(s, mtu), display(display), draw_commands(draw_commands) {};

    /**
     * Loop forever on incoming commands and run them.
     */
    void loop();
};
