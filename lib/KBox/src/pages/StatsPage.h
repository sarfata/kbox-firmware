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

#include "ui/Page.h"
#include "ui/TextLayer.h"

#include "tasks/NMEAReaderTask.h"
#include "tasks/NMEA2000Task.h"

class StatsPage : public Page {
  private:
    TextLayer *nmea1Rx, *nmea1Errors, *nmea2Rx, *nmea2Errors;
    TextLayer *canRx, *canTx, *canTxErrors;
    TextLayer *freeRam, *avgLoopTime;

    const TaskManager *taskManager;
    const NMEAReaderTask *reader1, *reader2;
    const NMEA2000Task *nmea2000Task;

    void loadView();

  public:
    StatsPage();
    bool processEvent(const TickEvent &e);

    void setTaskManager(const TaskManager *t) {
      taskManager = t;
    };

    void setNmea1Task(const NMEAReaderTask *t) {
      reader1 = t;
    };

    void setNmea2Task(const NMEAReaderTask *t) {
      reader2 = t;
    };

    void setNMEA2000Task(const NMEA2000Task *t) {
      nmea2000Task = t;
    };
};
