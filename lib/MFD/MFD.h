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

#include <stdint.h>

#include <Bounce.h>
#include <Encoder.h>
#include <Debug.h>
#include <List.h>

#include "GC.h"
#include "Display.h"
#include "ili9341display.h"
#include <font_Nunito.h>

enum EventType {
  EventTypeButton,
  EventTypeEncoder,
  EventTypeTick
};

class Event {
  private:
    EventType type;

  public:
    Event(EventType type) : type(type) {};
    EventType getEventType() const { return type; };
};

enum ButtonEventType {
  ButtonEventTypePressed,
  ButtonEventTypeReleased
};
class ButtonEvent : public Event {
  private:
  public:
    ButtonEvent() : Event(EventTypeButton) {};
    ButtonEvent(const ButtonEventType clickType) : Event(EventTypeButton), clickType(clickType) {};

    ButtonEventType clickType;
};

class EncoderEvent : public Event {
  public:
    EncoderEvent(int rot) : Event(EventTypeEncoder), rotation(rot) {};
    int rotation;
};

typedef unsigned long int time_ms_t;

class TickEvent : public Event {
  private:
    time_ms_t millis;

  public:
    TickEvent(unsigned long int millis) : Event(EventTypeTick), millis(millis) {};
    time_ms_t getMillis() const { return millis; };
};


class MFD;

/* A page is one simple app available on the MFD.
 *
 * - Only one page can be active at a time.
 * - Pages know how to draw themselves on a GC.
 * - Pages know how to process incoming events.
 */
class Page {
  private:

  public:
    virtual void willAppear() {};
    virtual void willDisappear() {};

    virtual bool processEvent(const ButtonEvent &e) {
      // By default button down will force switching to the next page.
      return !(e.clickType == ButtonEventTypePressed);
    }
    virtual bool processEvent(const EncoderEvent &e) {
      // By default this will be ignored.
      return true;
    }
    virtual bool processEvent(const TickEvent &e) {
      // By default this will be ignored.
      return true;
    };
    virtual bool processEvent(const Event &e) {
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
    };
    virtual void paint(GC &context) = 0;
    virtual ~Page() {};
};

/*
 * MFD or MultiFunctionDisplay manages multiple pages of information.
 *
 * - It keeps a list of available Pages
 * - It controls access to the display
 * - It dispatches incoming events to the active page
 *
 * Currently defines it's HID very strictly: one encoder, one button. We could
 * make that a bit more generic in the future.
 */
class MFD {
  protected:
    // Define tick duration in ms.
    static const int tickDuration = 200;
    Display &display;
    Encoder &encoder;
    Bounce &button;
    LinkedList<Page> pages;
    LinkedList<Event> events;
    unsigned long int lastTick;
    
    bool firstTick = true;

    CircularLinkedListIterator<Page> pageIterator;

    void processInputs();
    void processTicks();
    bool processEvent(Event *e);

  public:
    MFD(Display &display, Encoder &enc, Bounce &but);
    void loop();

    void addPage(Page *p) {
      pages.add(p);
    };
};



