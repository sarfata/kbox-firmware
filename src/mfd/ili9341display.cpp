#include "ili9341display.h"

#include <font_Nunito.h>

ILI9341Display::ILI9341Display(pin_t mosi, pin_t miso, pin_t sck, pin_t cs, pin_t dc, pin_t backlight, Size size)
  : size(size), backlightPin(backlight)
{
  display = new ILI9341_t3(cs, dc, 255 /* rst unused */, mosi, sck, miso);

  display->begin();
  display->fillScreen(ILI9341_BLUE);
  display->setRotation(3);

  setBacklight(BacklightIntensityMax);
}

void ILI9341Display::setBacklight(BacklightIntensity intensity) {
  if (backlightPin) {
    // TODO: Implement PWM of backlight.
    if (intensity > 0) {
      digitalWrite(backlightPin, 1);
    }
    else {
      digitalWrite(backlightPin, 0);
    }
  }
}

void ILI9341Display::draw_text(Point a, char *text) {
  display->setCursor(a.x(), a.y());
  display->setTextColor(ILI9341_WHITE);
  display->setTextSize(2);
  display->setFont(Nunito_18);
  display->println(text);
}

void ILI9341Display::draw_line(Point a, Point b, Color color) {
  display->drawLine(a.x(), a.y(), b.x(), b.y(), color);
}

void ILI9341Display::draw_rectangle(Point orig, Size size, Color color) {
  display->drawRect(orig.x(), orig.y(), size.width(), size.height(), color);
}

void ILI9341Display::fill_rectangle(Point orig, Size size, Color color) {
  display->fillRect(orig.x(), orig.y(), size.width(), size.height(), color);
}

