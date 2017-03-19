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

#include "KommandHandlerPing.h"
#include <KBoxLogging.h>

bool KommandHandlerPing::handleKommand(KommandReader &kreader, SlipStream &replyStream) {
  /* data here should be a 4 bytes packet identfier that we will include in the reply. */
  if (kreader.dataSize() != 4) {
    return false;
  }

  DEBUG("About to read!");


  uint32_t pingId = kreader.read32();

  DEBUG("About to send");
  FixedSizeKommand<4> pongFrame(KommandPong);
  pongFrame.append32(pingId);

  // Works if we return true here.
  DEBUG("pongFrame.getSize() == %u", pongFrame.getSize());
  replyStream.writeFrame(pongFrame.getBytes(), pongFrame.getSize());

  // crashes in sendReply. The reference to _slip is not valid.
  DEBUG("sent");

  return true;
}

