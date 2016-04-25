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

#include <List.h>
#include "GC.h"
#include "Event.h"
#include "Layer.h"

#include <Debug.h>
/* A page is one simple app available on the MFD.
 *
 * - Only one page can be active at a time.
 * - Pages know how to draw themselves on a GC.
 * - Pages know how to process incoming events.
 */
class BasePage {
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
    virtual bool processEvent(const Event &e);
    virtual void paint(GC &context) = 0;
    virtual ~BasePage() {};
};

/* BasePage offers a very generic paint implementation. 
 * Page assumes the use of Layers.
 *
 * Adding a layer via addLayer() will transfer ownership to the Page who will
 * destroy them when destroying itself.
 */
class Page : public BasePage {
  private:
    LinkedList<Layer *> _layers;
    bool isDirty;

  public:
    void willAppear();
    virtual ~Page();
    void addLayer(Layer *l);
    void paint(GC &context);
};

