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

#include "elapsedMillis.h"

class MfgTest {
  protected:
    String _name;
    String _instructions;
    String _reason;
    String _message;
    bool _finished;
    bool _success;
    unsigned int _timeout;

  public:
    MfgTest(const String& name, unsigned int timeout = 0) : _name(name), 
      _finished(false), _success(false), _timeout(timeout) {};


    const String& getName() const {
      return _name;
    };
    const String& getInstructions() const {
      return _instructions;
    };
    void setInstructions(const String& instructions) {
      _instructions = instructions;
    };

    void setMessage(const String& message) {
      _message = message;
    };
    const String & getMessage() const {
      return _message;
    };

    bool finished() {
      return _finished;
    };

    bool successful() {
      if (!_finished) {
        return false;
      }
      return _success;
    };

    const String& getReason() const {
      return _reason;
    };

    unsigned int getTimeout() const {
      return _timeout;
    };

    void pass() {
      _finished = true;
      _success = true;
    };
    void fail(const String &m = "") {
      _finished = true;
      _success = false;
      _reason = m;
    };

    virtual void setup() {};
    virtual void teardown() {};
    virtual void loop() = 0;
};

