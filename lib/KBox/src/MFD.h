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

#include <stdint.h>

#include <Bounce.h>
#include <Encoder.h>
#include <Debug.h>
#include <List.h>

#include "drivers/GC.h"
#include "drivers/Display.h"
#include "drivers/ili9341display.h"

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
    LinkedList<Page*> pages;
    LinkedList<Page*>::circularIterator pageIterator;
    LinkedList<Event> events;
    unsigned long int lastTick;
    
    bool firstTick = true;

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



