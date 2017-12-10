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

#include "KommandHandlerScreenshot.h"

KommandHandlerScreenshot::KommandHandlerScreenshot(GC &gc) : _gc(gc) {
}

bool KommandHandlerScreenshot::handleKommand(KommandReader &kreader, SlipStream &replyStream) {
  if (kreader.getKommandIdentifier() != KommandScreenshot) {
    return false;
  }

  // First byte is optional and can be where to start from.
  int y = 0;
  if (kreader.dataSize() >= 2) {
    y = kreader.read16();
  }

  // Allocating 320*5*2 = 3.2kB on stack. Ouch!
  static const int16_t lineWidth = 320;
  static const int16_t linePerFrame = 5;

  FixedSizeKommand<lineWidth * linePerFrame * 2> captureFrame(KommandScreenshotData);
  captureFrame.append16(y);

  uint8_t *bytes;
  size_t *index;
  captureFrame.captureBuffer(&bytes, &index);

  uint16_t *pixelData = (uint16_t*) (bytes + *index);

  _gc.readRect(0, y, lineWidth, linePerFrame, pixelData);
  *index += lineWidth * linePerFrame *2;

  replyStream.writeFrame(captureFrame.getBytes(), captureFrame.getSize());
  return true;
}
