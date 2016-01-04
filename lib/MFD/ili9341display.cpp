#include "ili9341display.h"

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
    if (intensity > 0) {
      analogWrite(backlightPin, intensity);
    }
    else {
      analogWrite(backlightPin, 0);
    }
  }
}

void ILI9341Display::drawText(Point a, Font font, Color color, const char *text) {
  display->setCursor(a.x(), a.y());
  display->setTextColor(color);
  display->setFont(Nunito_18);
  display->println(text);
}

void ILI9341Display::drawLine(Point a, Point b, Color color) {
  display->drawLine(a.x(), a.y(), b.x(), b.y(), color);
}

void ILI9341Display::drawRectangle(Point orig, Size size, Color color) {
  display->drawRect(orig.x(), orig.y(), size.width(), size.height(), color);
}

void ILI9341Display::fillRectangle(Point orig, Size size, Color color) {
  display->fillRect(orig.x(), orig.y(), size.width(), size.height(), color);
}

