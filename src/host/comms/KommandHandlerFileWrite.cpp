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
#include "KommandHandlerFileWrite.h"

bool KommandHandlerFileWrite::handleKommand(KommandReader &kreader,
                                            SlipStream &replyStream) {
  if (kreader.getKommandIdentifier() != KommandFileWrite) {
    return false;
  }

  uint32_t writeId = kreader.read32();
  uint32_t startPosition = kreader.read32();
  uint32_t size = kreader.read32();
  const char *filename = kreader.readNullTerminatedString();

  File f = KBox.getSdFat().open(filename, O_CREAT|O_WRITE);

  if (!f.isOpen()) {
    sendFileError(replyStream, writeId, KommandFileErrors::NoSuchFile);
    return true;
  }

  if (kreader.dataSize() - kreader.dataIndex() != size) {
    DEBUG("Invalid write size: %i-%i != %i", kreader.dataSize(),
          kreader.dataIndex(), size);
    sendFileError(replyStream, writeId, KommandFileErrors::InvalidWriteError);
    return true;
  }

  const uint8_t* writeData = kreader.dataBuffer() + kreader.dataIndex();
  f.seek(startPosition);

  // Documentation says this will return size or -1. Never anything else.
  if (f.write(writeData, size) != size) {
    sendFileError(replyStream, writeId, KommandFileErrors::WriteError);
    return true;
  }
  f.close();

  sendFileError(replyStream, writeId, KommandFileErrors::AOK);
  return true;
}
