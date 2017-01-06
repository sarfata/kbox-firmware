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
#include <List.h>
#include <KBoxLogging.h>

#include "ui/GC.h"
#include "drivers/Display.h"
#include "drivers/ili9341display.h"
#include "util/Task.h"
#include "ui/Page.h"
#include "ui/Event.h"
#include "ui/Page.h"

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
class MFD : public Task {
  protected:
    // Define tick duration in ms.
    static const int tickDuration = 200;
    Display &display;
    Encoder &encoder;
    Bounce &button;
    LinkedList<Page*> pages;
    LinkedList<Page*>::circularIterator pageIterator;
    LinkedList<Event*> events;
    unsigned long int lastTick;

    bool firstTick = true;

    void processInputs();
    void processTicks();
    bool processEvent(Event *e);

  public:
    MFD(Display &display, Encoder &enc, Bounce &but);

    void setup();
    void loop();

    void addPage(Page *p) {
      pages.add(p);
    };
};



