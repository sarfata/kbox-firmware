/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

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

#include "SDLogging.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <Seasmart.h>
#include <KBoxLogging.h>

static const int MAX_NMEA2000_MESSAGE_SEASMART_SIZE = 200;

SDLogging::SDLogging(fs::FS &fs) : _fs(fs) {
  _incomingQueue = xQueueCreate(500, sizeof(SDLoggingQueueItem));
}

void SDLogging::loop() {
  SDLoggingQueueItem item;

  uint32_t writtenLines = 0;
  uint32_t writtenBytes = 0;
  int64_t startTime = esp_timer_get_time();

  File file = _fs.open("/kbox32.log", FILE_APPEND);
  if (!file) {
    ERROR("Failed to open file for appending");
  }

  while (xQueueReceive(_incomingQueue, &(item), 0)) {
    //DEBUG("%s '%llu,%s'", file ? "Writing":"Discarding", item.timestamp, item.message);
    if (file) {
      int ret = file.printf("%llu,P,%s\n", item.timestamp, item.message);
      if (ret > 0) {
        writtenBytes += ret;
      }
      else {
        DEBUG("Write error %i", ret);
      }
    }

    // Message was copied with a call to strdup() - we must free the memory.
    free(item.message);
    writtenLines++;
  }
  if (file)
    file.close();

  int64_t timeSpent = esp_timer_get_time() - startTime;
  DEBUG("Wrote %i lines out of %u waiting in %p - (%u bytes) in %llums - %.2f kB/s",
           writtenLines, uxQueueMessagesWaiting(_incomingQueue), _incomingQueue,
        writtenBytes, timeSpent / 1000, writtenBytes / 1024.0 /
        (timeSpent / 1000000.0));
}

bool SDLogging::write(const tN2kMsg &msg) {
  char buf[MAX_NMEA2000_MESSAGE_SEASMART_SIZE];
  if (N2kToSeasmart(msg, xTaskGetTickCount(), buf, MAX_NMEA2000_MESSAGE_SEASMART_SIZE) == 0) {
    ERROR("n2k conversion error");
    return false;
  }
  else {
    SDLoggingQueueItem i;
    i.timestamp = xTaskGetTickCount() * portTICK_RATE_MS;
    i.message = strdup(buf);

    if (!xQueueSend(_incomingQueue, &i, (TickType_t) 0)) {
      ERROR("Unable to queue message into %p", _incomingQueue);
      free(i.message);
      return false;
    }
    else {
      return true;
    }
  }
}
