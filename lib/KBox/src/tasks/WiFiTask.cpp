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
#include "WiFiTask.h"
#include "KBoxDebug.h"
#include "../drivers/board.h"
#include "../drivers/esp8266.h"
#include "KMessageNMEAVisitor.h"

void WiFiTask::setup() {
  esp_init();
  DEBUG("booting ESP!");
  esp_reboot_in_program();
}

void WiFiTask::loop() {
  // Read data sent by WiFi module and print line by line
  while (WiFiSerial.available() > 0) {
    rxBuf[rxIndex++] = WiFiSerial.read();

    if (rxBuf[rxIndex - 1] == '\n') {
      rxBuf[rxIndex - 1] = 0;
      DEBUG("WiFi: %s", rxBuf);
      rxIndex = 0;
    }
    if (rxIndex >= sizeof(rxBuf) - 1) {
      rxBuf[rxIndex] = 0;
      DEBUG("WiFi full: %s", rxBuf);
      rxIndex = 0;
    }
  }

  // Try to send the first item from our queue (if possible)
  // Send data waiting in queue (as much as possible, we are limited by tx buffer size).
  while (sendQueue.size() || leftoverToSend.length()) {
    String toSend;
    if (leftoverToSend.length()) {
      toSend = leftoverToSend;
    }
    else {
      toSend = *(sendQueue.begin());
    }

    if (WiFiSerial.availableForWrite() <= 0) {
      //DEBUG("Not sending because send buffer is full (need %i but %i available).", toSend.length(), WiFiSerial.availableForWrite());
      break;
    }
    size_t available = WiFiSerial.availableForWrite();
    String s = toSend;
    s.remove(available);
    size_t written = WiFiSerial.print(s);
    //DEBUG("Sent %i/%i (%i avail) of %s", written, toSend.length(), available, toSend.c_str());

    if (written == toSend.length()) {
      if (leftoverToSend.length()) {
        leftoverToSend = "";
      }
      else {
        sendQueue.removeFirst();
      }
    }
    else {
      // If we did not have any leftover, then remove the entry from the queue
      // and save it as a leftover.
      // Otherwise, it was already removed from the queue.
      if (leftoverToSend.length() == 0) {
        sendQueue.removeFirst();
      }
      leftoverToSend = toSend.substring(written);
    }
  }
}

void WiFiTask::processMessage(const KMessage &m) {
  KMessageNMEAVisitor v;
  m.accept(v);
  sendQueue.add(v.toNMEA());
}


