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

#include "common/os/Task.h"
#include "common/algo/List.h"
#include "common/signalk/SKSubscriber.h"
#include "common/signalk/SKNMEAOutput.h"
#include "common/stats/KBoxMetrics.h"
#include "common/signalk/SKSource.h"
#include "common/signalk/SKHub.h"
#include "host/config/SerialConfig.h"

// Defined by the NMEA Standard
#define MAX_NMEA_SENTENCE_LENGTH 83

class HardwareSerial;

class SerialService : public Task, public SKSubscriber, private SKNMEAOutput {
  private:
    SerialConfig &_config;
    SKHub &_hub;
    HardwareSerial& stream;
    LinkedList<SKNMEASentence> receiveQueue;
    enum KBoxEvent _rxValidEvent, _rxErrorEvent, _txValidEvent, _txOverflowEvent;
    SKSourceInput _skSourceInput;
    LinkedList<SKNMEAOutput*> _repeaters;

  public:
    SerialService(SerialConfig &_config, SKHub &hub, HardwareSerial&s);

    void setup();
    void loop();
    void updateReceived(const SKUpdate&) override;
    bool write(const SKNMEASentence& nmeaSentence) override;
    void addRepeater(SKNMEAOutput &repeater);
};

