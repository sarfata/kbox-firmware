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

#include "GC.h"

class Layer {
  private:
    bool _dirty;
    Point _origin;
    Size _size;

  protected:
    void markDirty() {
      _dirty = true;
    }

  public:
    Layer(Point origin, Size size) : _dirty(true), _origin(origin), _size(size) { };
    virtual ~Layer() {};

    Point getOrigin() const {
      return _origin;
    };
    void setOrigin(const Point &p) {
      if (p != _origin) {
        _origin = p;
        markDirty();
      }
    };

    Size getSize() const {
      return _size;
    };
    void setSize(const Size &s) {
      if (s != _size) {
        _size = s;
        markDirty();
      }
    };

    bool isDirty() const {
      return _dirty;
    };

    virtual void paint(GC &gc) {
      _dirty = false;
    };
};
