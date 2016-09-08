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

#pragma once

#include <N2kMessages.h>
#include <NMEA2000_teensy.h>
#include <TaskManager.h>
#include "KMessage.h"

class NMEA2000Task : public Task, public KGenerator, public KReceiver, public KVisitor {
  private:
    tNMEA2000_teensy NMEA2000;
    unsigned int _rxValid, _txValid, _txErrors;

  public:
    NMEA2000Task() : Task("NMEA2000"), _rxValid(0), _txValid(0), _txErrors(0) {};
    void setup();
    void loop();

    // Helper for the handler who is not a part of this class
    void publishN2kMessage(const tN2kMsg& msg);

    void processMessage(const KMessage&);
    void visit(const NMEA2000Message&);

    unsigned int getRxValidCounter() const {
      return _rxValid;
    };
    unsigned int getTxValidCounter() const {
      return _txValid;
    };
    unsigned int getTxErrors() const {
      return _txErrors;
    };
};
