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

#include "WString.h"
#include "Layer.h"

class TextLayer : public Layer {
  private:
    String _text;
    Color _color;
    Color _bgColor;
    Font _font;

  public:
    TextLayer(const Point &origin, const Size &size, const String &text) : TextLayer(origin, size, text, ColorWhite, ColorBlack, FontDefault) {};
    TextLayer(const Point &origin, const Size &size, const String &text, const Color &color, const Color &backgroundColor, const Font &font) :
      Layer(origin, size), _text(text), _color(color), _bgColor(backgroundColor), _font(font) {};

    String getText() const { return _text; };
    void setText(const String &s);

    int getColor() const { return _color; };
    void setColor(const Color &color);

    Color getBackgroundColor() const { return _bgColor; };
    void setBackgroundColor(const Color &c);

    int getFont() const { return _font; };
    void setFont(const Font &font);

    virtual void paint(GC &gc);
};
