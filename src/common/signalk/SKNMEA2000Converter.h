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

#include <N2kMessages.h>
#include "common/signalk/SKUpdate.h"
#include "common/signalk/SKVisitor.generated.h"

class tN2kMsg;

class SKNMEA2000ConverterOutput {
  public:
    virtual ~SKNMEA2000ConverterOutput() {};
    virtual void pushMessage(const tN2kMsg& m) = 0;
};

/**
 * Converts one or multiple SignalK updates into a series of N2KMessages.
 */
class SKNMEA2000Converter : private SKVisitor {
  private:
    SKNMEA2000ConverterOutput *_currentOutput = 0;

    void generateWind(SKNMEA2000ConverterOutput &out, double windAngle, double windSpeed, tN2kWindReference windReference);

  protected:
    void visitSKElectricalBatteriesVoltage(const SKUpdate& u, const SKPath &p, const SKValue &v) override;

  public:
    /**
     * Process a SKUpdate and add messages to the internal queue of messages.
     */
    void convert(const SKUpdate& update, SKNMEA2000ConverterOutput& conversionOutput);
};
