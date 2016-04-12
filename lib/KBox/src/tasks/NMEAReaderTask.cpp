/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "NMEAReaderTask.h"
#include "Debug.h"
#include <Arduino.h>
#include <List.h>


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

NMEAReaderTask::NMEAReaderTask(HardwareSerial &s) : stream(s) {
  if (&s == &Serial2) {
    received2 = &receiveQueue;
  }
  if (&s == &Serial3) {
    received3 = &receiveQueue;
  }
}

void NMEAReaderTask::setup() {
}


void NMEAReaderTask::loop() {
  if (receiveQueue.size() == 0) {
    return;
  }
  // Send all queue sentences
  DEBUG("Found %i sentences waiting", receiveQueue.size());
  for (LinkedList<NMEASentence>::iterator it = receiveQueue.begin(); it != receiveQueue.end(); it++) {
    if (it->isValid()) {
      rxValidCounter++;
      this->sendMessage(*it);
    }
    else {
      rxErrorCounter++;
    }
  }
  // FIXME: the current linked list implementation would probably continue to
  // work if some data is added while we are running this loop but it would be
  // best if we had a clearer contract or a better way to manage this.
  receiveQueue.clear();
}

