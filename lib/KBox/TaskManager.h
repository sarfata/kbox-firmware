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
    IntervalTask(Task *t, uint32_t interval) : task(t), interval(interval), lastRun(0) {};

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

