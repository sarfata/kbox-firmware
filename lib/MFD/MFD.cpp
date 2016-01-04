#include "MFD.h"


MFD::MFD(Display & d, Encoder &e, Bounce &b) : display(d), encoder(e), button(b), pageIterator(pages.circularIterator()) {
  display.fillRectangle(Origin, display.getSize(), ColorBlue);
  lastTick = 0;
}

void MFD::processInputs() {
  /* Generate new events if needed */
  if (encoder.read() != 0) {
    Event *e = new EncoderEvent(encoder.read());
    DEBUG("Encoder rotated by %i", encoder.read());
    encoder.write(0);
    events.add(e);
  }
  if (button.update()) {
    Event *e;
    if (button.fallingEdge()) {
      DEBUG("Button down");
      e = new ButtonEvent(ButtonEventTypePressed);
    }
    else {
      DEBUG("Button up");
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

bool MFD::processEvent(Event *e) {
  switch (e->getEventType()) {
    case EventTypeButton:
      return pageIterator.current()->processEvent((ButtonEvent&) *e);
    case EventTypeEncoder:
      return pageIterator.current()->processEvent((EncoderEvent&) *e);
    case EventTypeTick:
      return pageIterator.current()->processEvent((TickEvent&) *e);
  }
  return false;
}

void MFD::loop() {
  //DEBUG("MFD loop");

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
  while ( (e = it.next()) ) {
    DEBUG("Processing one event of type: %i", e->getEventType());
    // Forward the event to the current page. If the handler returns false, skip to next page.
    if (!this->processEvent(e)) {
      DEBUG("Going to next page.");
      pageIterator.next();
    }
    delete(e);
  }
  events.emptyList();

  pageIterator.current()->paint(display);
}

