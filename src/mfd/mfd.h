#pragma once

#include <stdint.h>

class Point {
  private:
    uint16_t _x, _y;

  public:
    Point(uint16_t x, uint16_t y) : _x(x), _y(y) {};

    uint16_t x() const {
      return _x;
    };
    uint16_t y() const {
      return _y;
    };
};

static const Point Origin = Point(0, 0);

class Size {
  private:
    uint16_t _width, _height;

  public:
    Size(uint16_t w, uint16_t h) : _width(w), _height(h) {};

    uint16_t width() const {
      return _width;
    };

    uint16_t height() const {
      return _height;
    };
};

class Rectangle {
  private:
    Point _origin;
    Size _size;

  public:
    Rectangle(Point origin, Size size) : _origin(origin), _size(size) {};
    Rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) : _origin(x1, y1), _size(x2 - x1, y2 - y1) {};
};

typedef uint16_t Color;

// FIXME: Color definitions change with displays. This should be made display-dependent.
static const Color ColorBlue = 0x001F;
static const Color ColorWhite = 0xFFFF;
static const Color ColorBlack = 0x0000;

/* Graphics Context offers basic drawing primitives. */
class GC {
  public:
    virtual void draw_text(Point a, char *text) = 0;
    virtual void draw_line(Point a, Point b, Color color) = 0;
    virtual void draw_rectangle(Point orig, Size size, Color color) = 0;
    virtual void fill_rectangle(Point orig, Size size, Color color) = 0;
};

typedef uint16_t BacklightIntensity;
const BacklightIntensity BacklightIntensityMax = 1024;
const BacklightIntensity BacklightIntensityOff = 0;

class Display : public GC {
  public:
    // Returns the physical size of the display in pixels
    virtual const Size& getSize() const = 0;

    // Set backlight intensity
    virtual void setBacklight(BacklightIntensity intensity);
};

class Event {
};

class MFD;

/* A page is one simple app available on the MFD.
 *
 * - Only one page can be active at a time.
 * - Pages know how to draw themselves on a GC.
 * - Pages know how to process incoming events.
 */
class Page {
  protected:
    MFD &mfd;

  public:
    Page(MFD &mfd) : mfd(mfd) {
    };
    virtual void paint(GC &context) = 0;
};

struct PageLL {
  Page *page;
  struct PageLL *next;
};

typedef struct PageLL PageLL;

/*
 * MFD or MultiFunctionDisplay manages multiple pages of information.
 *
 * - It keeps a list of available Pages
 * - It controls access to the display
 * - It dispatches incoming events to the active page
 */
class MFD {
  protected:
    Display &display;
    PageLL *pages_list;

  public:
    MFD(Display &display) : display(display) {
      display.fill_rectangle(Origin, display.getSize(), ColorBlue);
    };
    void consume_event(Event e);

    void goto_next() {
      // paint the next page
    };
};


class SimplePage: Page {
  public:
    SimplePage(MFD &mfd) : Page(mfd) {};

    void process_click(void) {
      mfd.goto_next();
    };
};

