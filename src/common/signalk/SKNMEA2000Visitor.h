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

#pragma once

#include "common/algo/List.h"
#include "SKVisitor.generated.h"

class tN2kMsg;

/**
 * Converts one or multiple SignalK updates into a series of N2KMessages.
 *
 * N2kMsg are kept in an internal linked list and can be retrieved or flushed
 * at any point.
 */
class SKNMEA2000Visitor : SKVisitor {
  private:
    LinkedList<tN2kMsg*> _messages;

  protected:
    void visitSKElectricalBatteriesVoltage(const SKUpdate& u, const SKPath &p, const SKValue &v) override;
    void visitSKEnvironmentOutsidePressure(const SKUpdate& u, const SKPath &p, const SKValue &v) override;
    void visitSKNavigationAttitude(const SKUpdate& u, const SKPath &p, const SKValue &v) override;
    void visitSKNavigationHeadingMagnetic(const SKUpdate& u, const SKPath &p, const SKValue &v) override;
    void visitSKNavigationPosition(const SKUpdate& u, const SKPath &p, const SKValue &v) override;
    void visitSKNavigationSpeedOverGround(const SKUpdate& u, const SKPath &p, const SKValue &v) override;

  public:
    SKNMEA2000Visitor();
    ~SKNMEA2000Visitor();

    /**
     * Process a SKUpdate and add messages to the internal queue of messages.
     */
    void processUpdate(const SKUpdate& update) {
      visit(update);
    };

    /**
     * Retrieve the current list of messages.
     */
    const LinkedList<tN2kMsg*>& getMessages() const;

    /**
     * Flush the list of messages.
     */
    void flushMessages();
};
