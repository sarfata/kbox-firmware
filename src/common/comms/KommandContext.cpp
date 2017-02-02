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
#include <KBoxLogging.h>
#include "KommandContext.h"

KommandContext::KommandContext(SlipStream& slipStream, GC& gc) : _slip(slipStream), _gc(gc) {
}

void KommandContext::process(const uint8_t *bytes, size_t len) {
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
  Kommand<0> errorFrame(KommandErr);

  _slip.writeFrame(errorFrame.getBytes(), errorFrame.getSize());
}

void KommandContext::processPing(const uint8_t *data, size_t len) {
  /* data here should be a 4 bytes packet identfier that we will include in the reply. */
  if (len != 4) {
    sendErrorFrame();
    return;
  }

  uint32_t pingId = *((uint32_t*) data);

  Kommand<4> pongFrame(KommandPong);
  pongFrame.append32(pingId);
  _slip.writeFrame(pongFrame.getBytes(), pongFrame.getSize());
}

void KommandContext::processScreenshot(const uint8_t *data, size_t len) {
  // First byte is optional and can be where to start from.
  int y = 0;
  if (len == 2) {
    y = data[0] + (data[1] >> 8);
  }

  // Allocating 320*5*2 = 3.2kB on stack. Ouch!
  static const int16_t lineWidth = 320;
  static const int16_t linePerFrame = 5;

  Kommand<lineWidth * linePerFrame * 2> captureFrame(KommandScreenshotData);
  captureFrame.append16(y);

  uint8_t *bytes;
  size_t *index;
  captureFrame.captureBuffer(&bytes, &index);

  uint16_t *pixelData = (uint16_t*) (bytes + *index);

  _gc.readRect(0, y, lineWidth, linePerFrame, pixelData);
  *index += lineWidth * linePerFrame *2;

  _slip.writeFrame(captureFrame.getBytes(), captureFrame.getSize());
}
