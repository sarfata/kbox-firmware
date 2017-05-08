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

#include <KBoxLogging.h>
#include "KommandHandlerWiFiLog.h"

bool KommandHandlerWiFiLog::handleKommand(KommandReader &kreader, SlipStream &replyStream) {
  if (kreader.getKommandIdentifier() != KommandLog) {
    return false;
  }
  if (kreader.dataSize() < 6) {
    ERROR("Received log message too small to be valid - dataSize=%i", kreader.dataSize());
    return false;
  }

  KBoxLoggingLevel level = (KBoxLoggingLevel) kreader.read16();
  uint16_t lineno = kreader.read16();
  /* uint16_t fname_len = */ kreader.read16();

  const char *fname = kreader.readNullTerminatedString();
  if (fname == 0) {
    ERROR("Invalid log frame: no filename");
    return false;
  }

  const char *log = kreader.readNullTerminatedString();
  if (log == 0) {
    ERROR("Invalid log frame: no log (source %s:%i level:%i)", fname, lineno, level);
    return false;
  }


  // Convert the log level to report it as something we have received from ESP.
  switch (level) {
    case KBoxLoggingLevelDebug:
      level = KBoxLoggingLevelESPDebug;
      break;
    case KBoxLoggingLevelInfo:
      level = KBoxLoggingLevelESPInfo;
      break;
    case KBoxLoggingLevelError:
      level = KBoxLoggingLevelESPError;
      break;
    default:
      // Should not happen.
      level = KBoxLoggingLevelESPDebug;
      break;
  }

  // Actually log the message
  KBoxLogging.log(level, fname, lineno, log);

  return true;
}
