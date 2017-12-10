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

#include "ESPDebugLogger.h"
#include "comms/Kommand.h"

void ESPDebugLogger::log(enum KBoxLoggingLevel level, const char *fname, int lineno, const char *fmt, va_list fmtargs) {
  FixedSizeKommand<MaxLogFrameSize> logKmd(KommandLog);
  logKmd.append16(level);
  logKmd.append16(lineno);
  logKmd.append16(strlen(fname));
  logKmd.appendNullTerminatedString(fname);

  uint8_t *bytes;
  size_t *index;

  logKmd.captureBuffer(&bytes, &index);

  *index += vsnprintf((char*)bytes + *index, MaxLogFrameSize - *index, fmt, fmtargs);
  (*index)++; // for the null-terminating byte

  _slipStream.writeFrame(bytes, *index);
}
