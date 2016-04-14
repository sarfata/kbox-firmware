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

#include <Arduino.h> // for millis()
#include <stdint.h>
#include "List.h"

class Task {
  public:
    virtual ~Task() {};

    /**
     * Performs initial setup of the task. It is ok to take some time
     * to perform the initial setup (especially when initializing hardware,
     * etc)
     */
    virtual void setup() {};

    /**
     * Perform your work as fast as possible, do not block.
     */
    virtual void loop() = 0;
};

class IntervalTask : public Task {
  private:
    uint32_t interval;
    uint32_t lastRun;
    Task *task;

  public:
    IntervalTask(Task *t, uint32_t interval) : interval(interval),lastRun(0) {
      task = t;
    };

    ~IntervalTask() {
      delete(task);
    };

    void setup() {
      task->setup();
    };

    void loop() {
      if (millis() > lastRun + interval) {
        task->loop();
        lastRun = millis();
      }
    };
};



class TaskManager {
  public:
    TaskManager() {
    };

    void addTask(Task* task) {
      tasks.add(task);
    };

    void setup() {
      for (LinkedList<Task*>::iterator it = tasks.begin(); it != tasks.end(); it++) {
        (*it)->setup();
      }
    };

    void loop() {
      for (LinkedList<Task*>::iterator it = tasks.begin(); it != tasks.end(); it++) {
        (*it)->loop();
      }
    };

  private:
    LinkedList<Task*> tasks;
};

