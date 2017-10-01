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
#include <KBoxHardware.h>
#include "common/signalk/SKNMEAVisitor.h"
#include "common/signalk/KMessageNMEAVisitor.h"
#include "common/signalk/SKJSONVisitor.h"
#include "stats/KBoxMetrics.h"

#include "WiFiService.h"

WiFiService::WiFiService(SKHub &skHub, GC &gc) : Task("WiFi"), _hub(skHub), _slip(WiFiSerial, 2048) {
  // We will need gc at some point to be able to take screenshot
}

void WiFiService::setup() {
  KBox.espInit();
  DEBUG("booting ESP!");
  KBox.espRebootInProgram();

  _hub.subscribe(this);
}

void WiFiService::loop() {
  if (_slip.available()) {
    uint8_t *frame;
    size_t len = _slip.peekFrame(&frame);

    KommandReader kr = KommandReader(frame, len);

    KommandHandler *handlers[] = { &_pingHandler, &_wifiLogHandler, 0 };
    if (KommandHandler::handleKommandWithHandlers(handlers, kr, _slip)) {
      KBoxMetrics.event(KBoxEventWiFiValidKommand);
    }
    else {
      KBoxMetrics.event(KBoxEventWiFiInvalidKommand);
      ERROR("Invalid command received from WiFi module (id=%i size=%i)", kr.getKommandIdentifier(), kr.dataSize());
      frame[len-1] = 0;
      DEBUG("> %s", frame);
    }

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
  FixedSizeKommand<256> k(KommandNMEASentence);
  k.appendNullTerminatedString(data.c_str());

  _slip.writeFrame(k.getBytes(), k.getSize());
}

void WiFiService::updateReceived(const SKUpdate& u) {
  /* This is where we convert the data in SignalK format that floats inside KBox
   * to messages that will be sent to WiFi clients.
   */

  // Convert to NMEA format first
  SKNMEAVisitor nmeaVisitor;
  nmeaVisitor.processUpdate(u);
  for (LinkedList<String>::constIterator it = nmeaVisitor.getSentences().begin(); it != nmeaVisitor.getSentences().end(); it++) {
    // NMEA Sentences should always be 82 bytes or less
    FixedSizeKommand<100> k(KommandNMEASentence);
    k.appendNullTerminatedString((*it).c_str());
    _slip.writeFrame(k.getBytes(), k.getSize());
  }

  // Now send in JSON format
  StaticJsonBuffer<1024> jsonBuffer;
  SKJSONVisitor jsonVisitor(jsonBuffer);
  JsonObject &jsonData = jsonVisitor.processUpdate(u);
  FixedSizeKommand<1024> k(KommandSKData);
  jsonData.printTo(k);
  //FIXME: who adds the final 0 here?
  _slip.writeFrame(k.getBytes(), k.getSize());
}

