/*
  The MIT License

  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net

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

#include <stdlib.h>
#include "KommandContext.h"

KommandContext::KommandContext(SlipStream& slipStream, GC& gc) : _slip(slipStream), _gc(gc) {
}

void KommandContext::process(const uint8_t *bytes, size_t len) {
  /*
   * Frame format:
   * <identifier> <data>
   * 0x00: KMD_PING => No data. Respond with PONG frame.
   * 0x01: KMD_PONG => No data. Sent in response to PING command.
   *
   * 0x0F: KMD_ERR => Error (frame not understood in current context).
   *
   * 0x20: KMD_READFILE => Start reading file. data is the path to the data to read.
   * 0x21: KMD_READFILE_DATA => Response used to send file data back <filesize 4bytes> <data>
   * 0x22: KMD_READFILE_CONT(x 4 bytes) => Continue reading from offset x
   * 0x2F: KMD_READFILE_ERR => Error reading file. <error code 1byte> <error message string>
   *
   * 0x30: KMD_SCREENSHOT => Start a screenshot.
   * 0x31: KMD_SCREENSHOT_DATA => send screenshot data: <y coordinate 1byte> <pixel data in 16bit per pixels>
   * 0x32: KMD_SCREENSHOT_CONT(y 1 byte) => Continue screenshot from offset y
   *
   * 0x33: KMD_SCREENPUSH => Start pushing data to screen
   * 0x34: KMD_SCREENPUSH_DATA => screenshot data: <y coordinate 1 byte> <pixel data in 16bit per pixels>
   */
  if (len == 0) {
    // should never happen because empty slip frames cannot be transmitted.
    return;
  }

  uint16_t identifier = bytes[0] + (bytes[1]<<8);
  bytes = bytes + 2;
  len = len - 2;

  switch (identifier) {
    case KommandPing:
      processPing(bytes, len);
      break;
    case KommandScreenshot:
      processScreenshot(bytes, len);
      break;
    default:
      // Send error frame.
      sendErrorFrame();
  };
};

void KommandContext::sendErrorFrame() {
  Kommand<2> errorFrame(KommandErr);

  _slip.writeFrame(errorFrame.getBytes(), errorFrame.getSize());
}

void KommandContext::processPing(const uint8_t *data, size_t len) {
  /* data here should be a 4 bytes packet identfier that we will include in the reply. */
  if (len != 4) {
    sendErrorFrame();
    return;
  }

  uint32_t pingId = *((uint32_t*) data);

  Kommand<6> pongFrame(KommandPong);
  pongFrame.append32(pingId);
  _slip.writeFrame(pongFrame.getBytes(), pongFrame.getSize());
}

void KommandContext::processScreenshot(const uint8_t *data, size_t len) {
  // First byte is optional and can be where to start from.
  int y = 0;
  if (len > 0) {
    y = data[0];
  }

  static const int16_t lineWidth = 320;
  static const int16_t linePerFrame = 5;

  size_t replyLen = 2 + 2 * (lineWidth * linePerFrame);
  uint8_t *reply = (uint8_t*) malloc(replyLen);

  if (reply == 0) {
    sendErrorFrame();
  }
  else {
    reply[0] = KommandScreenshotData;
    reply[1] = y;
    uint16_t *pcolors = (uint16_t*) (reply + 2);
    _gc.readRect(0, y, lineWidth, linePerFrame, pcolors);
    _slip.writeFrame(reply, replyLen);
    free(reply);
  }
}
