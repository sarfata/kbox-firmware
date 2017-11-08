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
#include <Arduino.h>
#include "common/algo/List.h"
#include "common/stats/KBoxMetrics.h"
#include "common/signalk/SKNMEAParser.h"
#include "NMEAService.h"


// Linked list used to buffer messages
static LinkedList<NMEASentence> *received2 = 0;
static LinkedList<NMEASentence> *received3 = 0;

// This is called by yield() whenever data is available.
// We move received data into a linked list of NMEA sentences.
void serialEvent2() {
  static uint8_t buffer[MAX_NMEA_SENTENCE_LENGTH];
  static int index = 0;

  if (received2 == 0) {
    return;
  }

  // 64 is the RX_BUFFER_SIZE defined in serial2.c (teensy3 framework)
  if (Serial2.available() == 64) {
    KBoxMetrics.event(KBoxEventNMEA1RXBufferOverflow);
    DEBUG("serialEvent2 found a full rx buffer - we probably lost some data");
  }

  while (Serial2.available()) {
    buffer[index++]= (uint8_t) Serial2.read();

    // Check if we are at the end of the buffer
    // The -1 is because we need space to add a terminating NULL character.
    if (index >= MAX_NMEA_SENTENCE_LENGTH - 1) {
      buffer[MAX_NMEA_SENTENCE_LENGTH - 1] = 0;
      DEBUG("Discarding incomplete sequence: %s", buffer);
      index = 0;
    }
    else {
      // Check if we have reached end of sentence
      if (buffer[index-1] == '\r' || buffer[index-1] == '\n') {
        if (index > 1) {
          // Only send message if it's greater than 0 bytes.
          // And we know that index is < MAX_NMEA_SENTENCE_LENGTH
          // because we tested buffer.
          buffer[index-1] = 0;
          NMEASentence s((char*)buffer);
          received2->add(s);
        }
        // Start again from scratch
        index = 0;
      }
    }
  }
}

void serialEvent3() {
  static uint8_t buffer[MAX_NMEA_SENTENCE_LENGTH];
  static int index = 0;

  if (received3 == 0) {
    return;
  }

  // 64 is the RX_BUFFER_SIZE defined in serial3.c (teensy3 framework)
  if (Serial3.available() == 64) {
    KBoxMetrics.event(KBoxEventNMEA2RXBufferOverflow);
    DEBUG("serialEvent3 found a full rx buffer - we probably lost some data");
  }

  while (Serial3.available()) {
    buffer[index++]= (uint8_t) Serial3.read();

    // Check if we are at the end of the buffer
    // The -1 is because we need space to add a terminating NULL character.
    if (index >= MAX_NMEA_SENTENCE_LENGTH - 1) {
      buffer[MAX_NMEA_SENTENCE_LENGTH - 1] = 0;
      DEBUG("Discarding incomplete sequence: %s", buffer);
      index = 0;
    }
    else {
      // Check if we have reached end of sentence
      if (buffer[index-1] == '\r' || buffer[index-1] == '\n') {
        if (index > 1) {
          // Only send message if it's greater than 0 bytes.
          // And we know that index is < MAX_NMEA_SENTENCE_LENGTH
          // because we tested buffer.
          buffer[index-1] = 0;
          NMEASentence s((char*)buffer);
          received3->add(s);
        }
        // Start again from scratch
        index = 0;
      }
    }
  }
}

NMEAService::NMEAService(SKHub &hub, HardwareSerial &s) : Task("NMEA Service"), _hub(hub), stream(s) {
  if (&s == &Serial2) {
    received2 = &receiveQueue;
    _taskName = "NMEA Reader1";
    rxValidEvent = KBoxEventNMEA1RX;
    rxErrorEvent = KBoxEventNMEA1RXError;
    _skSourceInput = SKSourceInputNMEA0183_1;
  }
  if (&s == &Serial3) {
    received3 = &receiveQueue;
    _taskName = "NMEA Reader2";
    rxValidEvent = KBoxEventNMEA2RX;
    rxErrorEvent = KBoxEventNMEA2RXError;
    _skSourceInput = SKSourceInputNMEA0183_2;
  }
}

void NMEAService::setup() {
}


void NMEAService::loop() {
  if (receiveQueue.size() == 0) {
    return;
  }
  // Send all queue sentences
  DEBUG("Found %i sentences waiting", receiveQueue.size());
  for (LinkedList<NMEASentence>::iterator it = receiveQueue.begin(); it != receiveQueue.end(); it++) {
    if (it->isValid()) {
      KBoxMetrics.event(rxValidEvent);
      this->sendMessage(*it);

      SKNMEAParser p;
      //FIXME: Get the time properly here!
      const SKUpdate &update = p.parse(_skSourceInput, (*it).getSentence(), SKTime(0));
      if (update.getSize() > 0) {
        _hub.publish(update);
      }
    }
    else {
      KBoxMetrics.event(rxErrorEvent);
    }
  }
  // FIXME: the current linked list implementation would probably continue to
  // work if some data is added while we are running this loop but it would be
  // best if we had a clearer contract or a better way to manage this.
  receiveQueue.clear();
}
