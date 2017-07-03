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
#include "MFD.h"

// Disable debug from MFD.
#undef DEBUG
#define DEBUG(...) /* */

MFD::MFD(GC &gc, Encoder &e, Bounce &b) : Task("MFD"), gc(gc), encoder(e), button(b), pageIterator(pages.circularBegin()) {
}

void MFD::setup() {
  gc.fillRectangle(Origin, gc.getSize(), ColorBlack);
  lastTick = 0;
}

void MFD::processInputs() {
  /* Generate new events if needed */
  if (encoder.read() != 0) {
    Event *e = new EncoderEvent(encoder.read());
    encoder.write(0);
    events.add(e);
  }
  if (button.update()) {
    if (button.fallingEdge()) {
      events.add(new ButtonEvent(ButtonEventTypePressed));
    }
    else {
      events.add(new ButtonEvent(ButtonEventTypeReleased));
    }
  }
}

void MFD::processTicks() {
  if (millis() > lastTick + tickDuration) {
    TickEvent *e = new TickEvent(millis());
    events.add(e);
    lastTick = e->getMillis();
  }
}

void MFD::loop() {
  if (pageIterator == pages.circularEnd()) {
    if (pages.size() > 0) {
      pageIterator = pages.circularBegin();
    }
    else {
      DEBUG("No current page!");
      // bail out here and hope we will have a valid page next time.
      return;
    }
  }

  // TODO: find a better way to do this.
  if (firstTick) {
    DEBUG("firstTick!");
    (*pageIterator)->willAppear();
    firstTick = false;
  }

  this->processInputs();
  this->processTicks();

  if (events.size() > 0) {
    DEBUG("Processing %i events", events.size());
  }
  for (LinkedList<Event*>::iterator it = events.begin(); it != events.end(); it++) {
    // Forward the event to the current page. If the handler returns false, skip to next page.
    if (!(*pageIterator)->processEvent(**it)) {
      DEBUG("Going to next page.");
      (*pageIterator)->willDisappear();
      pageIterator++;
      (*pageIterator)->willAppear();
    }
    delete(*it);
  }
  events.clear();

  (*pageIterator)->paint(gc);
}

