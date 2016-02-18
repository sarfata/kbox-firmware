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

#include "MFD.h"

#undef DEBUG
#define DEBUG(...) /* */

MFD::MFD(Display & d, Encoder &e, Bounce &b) : display(d), encoder(e), button(b), pageIterator(pages.circularIterator()) {
  display.fillRectangle(Origin, display.getSize(), ColorBlue);
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
    Event *e;
    if (button.fallingEdge()) {
      e = new ButtonEvent(ButtonEventTypePressed);
    }
    else {
      e = new ButtonEvent(ButtonEventTypeReleased);
    }
    events.add(e);
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
  // Make sure we have a current page.
  if (!pageIterator.current()) {
    DEBUG("No current page!");
    // bail out here and hope we will have a valid page next time.
    return;
  }

  // TODO: find a better way to do this.
  if (firstTick) {
    DEBUG("firstTick!");
    pageIterator.current()->willAppear();
    firstTick = false;
  }

  this->processInputs();
  this->processTicks();

  LinkedListIterator<Event> it = events.iterator();
  Event *e;
  if (events.size() > 0) {
    DEBUG("Processing %i events", events.size());
  }
  while ( (e = it.next()) ) {
    // Forward the event to the current page. If the handler returns false, skip to next page.
    if (!pageIterator.current()->processEvent(*e)) {
      DEBUG("Going to next page.");
      pageIterator.current()->willDisappear();
      pageIterator.next();
      pageIterator.current()->willAppear();
    }
    delete(e);
  }
  events.emptyList();

  pageIterator.current()->paint(display);
}

