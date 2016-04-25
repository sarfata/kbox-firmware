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

#include "Page.h"
#include <Debug.h>

bool BasePage::processEvent(const Event &e) {
  switch (e.getEventType()) {
    case EventTypeButton:
      return this->processEvent((ButtonEvent&) e);
    case EventTypeEncoder:
      return this->processEvent((EncoderEvent&) e);
    case EventTypeTick:
      return this->processEvent((TickEvent&) e);
    default:
      DEBUG("Unsupported event.");
      return false;
  }
}

Page::~Page() {
  for (LinkedList<Layer*>::iterator it = _layers.begin(); it != _layers.end(); it++) {
    delete((*it));
  }
}

void Page::addLayer(Layer *l) {
  _layers.add(l);
}

void Page::paint(GC &gc) {
  for (LinkedList<Layer*>::iterator it = _layers.begin(); it != _layers.end(); it++) {
    if ((*it)->isDirty()) {
      (*it)->paint(gc);
    }
  }
}

