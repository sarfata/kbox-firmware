#pragma once

#include <ILI9341_t3.h>
#include "mfd.h"
#include "../hardware/pin.h"

class ILI9341Display : public Display {
  private:
    ILI9341_t3 *display;
    Size size;
    pin_t backlightPin;

  public:
    ILI9341Display(pin_t mosi, pin_t miso, pin_t sck, pin_t cs, pin_t dc, pin_t backlight, Size size);

    /* Display interface */
    const Size& getSize() const {
      return size;
    }

    void setBacklight(BacklightIntensity intensity);

    /* GC interface */
    void draw_text(Point a, char *text);
    void draw_line(Point a, Point b, Color color);
    void draw_rectangle(Point orig, Size size, Color color);
    void fill_rectangle(Point orig, Size size, Color color);
};
