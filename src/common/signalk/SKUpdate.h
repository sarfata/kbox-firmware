/*
  The MIT License

  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net

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

#include "SKSource.h"
#include "SKValue.h"

template <int capacity> class SKUpdate {
  private:
    SKSource _source = SKSourceUnknown;
    SKValue _values[capacity];
    int _size = 0;

  public:
    /**
     * Create a new SKUpdate object with an empty list that can hold at most
     * `size` values.
     */
    SKUpdate() {
    };

    /**
     * Add a new SKValue to this update. This will fail if the list is full.
     *
     * @return: true if succeeded, or false if the list is full.
     */
    bool addValue(SKValue &v) {
      if (_size < capacity) {
        _values[_size] = v;
        _size++;
        return true;
      } else {
        return false;
      }
    };

    /**
     * Returns the number of values in this update (it might be less than the
     * capacity).
     */
    int getSize() const {
      return _size;
    };

    /**
     * Returns the source of the values in this update.
     */
    const SKSource& getSource() {
      return _source;
    };

    /**
     * Sets the source of the values in this update.
     */
    void setSource(const SKSource& source) {
      _source = source;
    };

    /**
     * Return a value by index.
     */
    const SKValue& operator[] (int index) const {
      if (index >= _size) {
        return SKValueNone;
      }
      return _values[index];
    }

    /**
     * Return a value by path, or null if the update does not have it.
     */
    const SKValue& operator[] (SKPath path) const {
      for (int i = 0; i < _size; i++) {
        if (_values[i].getPath() == path) {
          return _values[i];
        }
      }
      return SKValueNone;
    }
};
