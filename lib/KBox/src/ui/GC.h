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
#include <WString.h>

#include "Point.h"
#include "Size.h"
#include "Rect.h"
#include "Color.h"
#include "Font.h"

/* Graphics Context offers basic drawing primitives. */
class GC {
  public:
    virtual void drawText(Point a, Font font, Color color, const char *text) = 0;
    virtual void drawText(Point a, Font font, Color color, Color bgColor, const char *text) = 0;
    virtual void drawText(const Point &a, const Font &font, const Color &color, const Color &bgColor, const String &text) = 0;
    virtual void drawLine(Point a, Point b, Color color) = 0;
    virtual void drawRectangle(Point orig, Size size, Color color) = 0;
    virtual void fillRectangle(Point orig, Size size, Color color) = 0;

    virtual void readRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors) = 0;

    void drawRectangle(const Rect& rect, Color color) {
      drawRectangle(rect.origin(), rect.size(), color);
    };

    void fillRectangle(const Rect& rect, Color color) {
      fillRectangle(rect.origin(), rect.size(), color);
    };
};
