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

#include <N2kMsg.h>
#include <N2kMessages.h>
#include "SKNMEA2000Visitor.h"
#include "SKUpdate.h"
#include "SKValue.h"

SKNMEA2000Visitor::SKNMEA2000Visitor() {
}

SKNMEA2000Visitor::~SKNMEA2000Visitor() {
  flushMessages();
}

void SKNMEA2000Visitor::processUpdate(const SKUpdate& update) {
  // PGN 129026: Fast COG/SOG
  if (update[SKPathNavigationSpeedOverGround] != SKValueNone
      && update[SKPathNavigationCourseOverGroundTrue] != SKValueNone) {
    tN2kMsg *msg = new tN2kMsg();
    SetN2kPGN129026(*msg, (uint8_t)0, N2khr_true,
        update[SKPathNavigationCourseOverGroundTrue].getNavigationCourseOverGroundTrue(),
        update[SKPathNavigationSpeedOverGround].getNavigationSpeedOverGround());

    _messages.add(msg);
  }
}

const LinkedList<tN2kMsg*>& SKNMEA2000Visitor::getMessages() const {
  return _messages;
}

void SKNMEA2000Visitor::flushMessages() {
  for (LinkedListIterator<tN2kMsg*> it = _messages.begin(); it != _messages.end(); it++) {
    delete (*it);
  }
  _messages.clear();
}
