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

MFD::MFD(Display & d, Encoder &e, Bounce &b) : display(d), encoder(e), button(b), pageIterator(pages.circularBegin()) {
  display.fillRectangle(Origin, display.getSize(), ColorBlue);
  lastTick = 0;
}

void MFD::processInputs() {
  /* Generate new events if needed */
  if (encoder.read() != 0) {
    Event e = EncoderEvent(encoder.read());
    encoder.write(0);
    events.add(e);
  }
  if (button.update()) {
    if (button.fallingEdge()) {
      events.add(ButtonEvent(ButtonEventTypePressed));
    }
    else {
      events.add(ButtonEvent(ButtonEventTypeReleased));
    }
  }
}

void MFD::processTicks() {
  if (millis() > lastTick + tickDuration) {
    TickEvent e = TickEvent(millis());
    events.add(e);
    lastTick = e.getMillis();
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

  LinkedList<Event>::iterator it = events.begin();
  if (events.size() > 0) {
    DEBUG("Processing %i events", events.size());
  }
  for (LinkedList<Event>::iterator it = events.begin(); it != events.end(); it++) {
    // Forward the event to the current page. If the handler returns false, skip to next page.
    if ((*pageIterator)->processEvent(*it)) {
      DEBUG("Going to next page.");
      (*pageIterator)->willDisappear();
      pageIterator++;
      (*pageIterator)->willAppear();
    }
  }
  events.clear();

  (*pageIterator)->paint(display);
}

