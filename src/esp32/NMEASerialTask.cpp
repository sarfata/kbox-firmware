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

#include <KBoxLogging.h>
#include "NMEASerialTask.h"

// TX Pins tested on both Serial instances
// GPIO      PIN#onCan32      Serial1      Serial2

// 25        15               X            X
// 14        18               X            X     -- did not work with WROVER-KIT // probably because it is hs2_clock too
// 21        SDA              X            X
// 27                                      X



// 12                         -                     // also hs2_data2 - might explain why it does not work on wrover
// 13
//                                     -


// DAH! Those below are clearly marked as input only on the datasheet!
// 34        10               -
// 35        11               -            -
// 36        SVP              -
// 39        SVN                           -

NMEASerialTask::NMEASerialTask(int identifier, uint8_t txPin, uint8_t rxPin, int baudRate) :
  _identifier(identifier), _serialPort(identifier) {
  _serialPort.begin(baudRate, SERIAL_8N1, rxPin, txPin, false);
}

bool NMEASerialTask::write(const char *nmeaSentence) {
  if (_serialPort.availableForWrite() < strlen(nmeaSentence) + 1) {
    return false;
  } else {
    _serialPort.write(nmeaSentence);
    _serialPort.write("\n");
    return true;
  }
}

const char *NMEASerialTask::readLine() {
  while (_serialPort.available() && _index < sizeof(_buffer)) {
    _buffer[_index++] = _serialPort.read();

    // Have we reached an end of line?
    if (_buffer[_index - 1] == '\n') {
      _buffer[_index - 1] = '\0';
      _index = 0;
      return _buffer;
    }
  }
  if (_index >= sizeof(_buffer)) {
    _buffer[sizeof(_buffer) - 1] = '\0';
    DEBUG("Discarding data: %s", _buffer);
    _index = 0;
  }
  return nullptr;
}