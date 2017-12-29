/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

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

#include <KBoxLogging.h>
#include <KBoxHardware.h>
#include "KommandHandlerFileRead.h"

bool KommandHandlerFileRead::handleKommand(KommandReader &kreader, SlipStream &replyStream) {
  if (kreader.getKommandIdentifier() != KommandFileRead) {
    return false;
  }

  uint32_t readId = kreader.read32();
  uint32_t startPosition = kreader.read32();
  uint32_t size = kreader.read32();
  const char *filename = kreader.readNullTerminatedString();

  if (KBox.getSdFat().exists(filename)) {
    File f = KBox.getSdFat().open(filename, O_READ);

    if (size > f.fileSize()) {
      size = f.fileSize();
    }
    if (size > MaxReadSize) {
      size = MaxReadSize;
    }

    f.seekSet(startPosition);

    FixedSizeKommand<MaxReadSize + 2*4> replyFrame(KommandFileReadReply);
    replyFrame.append32(readId);

    uint8_t *bytes;
    size_t *index;
    replyFrame.captureBuffer(&bytes, &index);

    // We need to set the size *after* we read the data
    size_t indexBeforeAddingSize = *index;

    // fake add size
    replyFrame.append32(0);

    int readCount = f.read(bytes + *index, size);
    size_t frame_size = *index + readCount;

    // Update the 'size' field with how many bytes were actually read
    *index = indexBeforeAddingSize;
    replyFrame.append32(readCount);

    *index = frame_size;

    replyStream.writeFrame(replyFrame.getBytes(), replyFrame.getSize());
  }
  else {
    sendFileError(replyStream, readId, KommandFileErrors::NoSuchFile);
  }
  return true;
}
