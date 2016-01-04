#pragma once

#include <ILI9341_t3.h>
#include "MFD.h"
#include "pin.h"

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
    void drawText(Point a, Font font, Color color, const char *text);
    void drawLine(Point a, Point b, Color color);
    void drawRectangle(Point orig, Size size, Color color);
    void fillRectangle(Point orig, Size size, Color color);
};
