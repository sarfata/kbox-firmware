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

#include <KBoxLogging.h>
#include "WiFiService.h"
#include "../drivers/board.h"
#include "../drivers/esp8266.h"
#include "KMessageNMEAVisitor.h"

WiFiService::WiFiService() : Task("WiFi"), _slip(WiFiSerial, 2048), _kommandContext(_slip, KBox.getDisplay()) {
}

void WiFiService::setup() {
  esp_init();
  DEBUG("booting ESP!");
  esp_reboot_in_program();
}

void WiFiService::loop() {
  if (_slip.available()) {
    uint8_t *frame;
    size_t len = _slip.peekFrame(&frame);

    _kommandContext.process(frame, len);

    _slip.readFrame(0, 0);
  }
}

void WiFiService::processMessage(const KMessage &m) {
  KMessageNMEAVisitor v;
  m.accept(v);

  String data = v.toNMEA();

  // TODO
  // Should not limit like this to only 256 because there could be many nmea lines in this message.
  // Let's figure this out when we pass SignalK data.
  Kommand<256> k(KommandNMEASentence);
  k.appendNullTerminatedString(data.c_str());

  _slip.writeFrame(k.getBytes(), k.getSize());
}


