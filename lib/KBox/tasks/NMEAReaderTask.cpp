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

void NMEAReaderTask::setup() {
  index = 0;
}

void NMEAReaderTask::loop() {
  // We read and look for our separator '\r\n' at the same time
  // readBytes() and other also call read in a loop so this is
  // probably more efficient.
  while (stream.available()) {
    buffer[index++] = (uint8_t)stream.read();

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
          buffer[index] = 0;
          //DEBUG("NMEA: %s", buffer);
          NMEASentence s((char*)buffer);
          this->sendMessage(s);
        }
        // Start again from scratch.
        index = 0;
      }
    }
  }
}

