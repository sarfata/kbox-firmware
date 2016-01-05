#include "MFD.h"


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
    pageIterator.next();
    // bail out here and hope we will have a valid page next time.
    return;
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
      pageIterator.next();
    }
    delete(e);
  }
  events.emptyList();

  pageIterator.current()->paint(display);
}

