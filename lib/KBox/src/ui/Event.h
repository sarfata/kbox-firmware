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
  // Those two events are always sent when button goes up/down
  ButtonEventTypePressed,
  ButtonEventTypeReleased,

  // Those events are also sent when a single click, or a double click happen
  ButtonEventTypeClick,
  ButtonEventTypeLongClick,

  // This event is sent multiple times, as long as button is maintained
  ButtonEventTypeMaintained,
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


