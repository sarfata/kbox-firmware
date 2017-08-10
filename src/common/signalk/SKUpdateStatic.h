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

#include "SKUpdate.h"
#include "SKPath.h"

/**
 * Implements a statically allocated SKUpdate. The capacity of the update is
 * defined when instantiating the object.
 */
template <uint16_t capacity> class SKUpdateStatic : public SKUpdate {
  private:
    SKSource _source = SKSourceUnknown;
    SKTime _timestamp;
    const SKContext& _context;

    // Very simple hash-type data structure
    SKPath _paths[capacity];
    SKValue _values[capacity];
    uint16_t _size = 0;

  public:
    /**
     * Create a new SKUpdate object, with the context 'self' and  with an empty
     * list that can hold at most `size` values.
     */
    SKUpdateStatic() : _context(SKContextSelf) {};

    /**
     * Create a new SKUpdate object, with the given context and with an empty
     * list that can hold at most `size` values.
     */
    SKUpdateStatic(const SKContext& context) : _context(context) {};

    ~SKUpdateStatic() {};

    virtual bool hasPath(const SKPath &p) const override {
      for (uint16_t i = 0; i < _size; i++) {
        if (_paths[i] == p) {
          return true;
        }
      }
      return false;
    };

    virtual bool setValue(const SKPath p, const SKValue v) override {
      // Update?
      for (uint16_t i = 0; i < _size && i < capacity; i++) {
        if (_paths[i] == p) {
          _values[i] = v;
          return true;
        }
      }
      // Add?
      if (_size < capacity) {
        _paths[_size] = p;
        _values[_size] = v;
        _size++;
        return true;
      }
      return false;
    };

    /**
     * Returns the source of the values in this update.
     */
    void setSource(const SKSource& source) {
      _source = source;
    };

    const SKTime& getTimestamp() const override {
      return _timestamp;
    };

    void setTimestamp(const SKTime& timestamp) {
      _timestamp = timestamp;
    };

    uint16_t getSize() const override {
      return _size;
    };

    int getSizeBytes() const override {
      // Should add the size of all referenced values.
      return sizeof(this);
    };

    const SKSource& getSource() const override {
      return _source;
    };

    const SKContext& getContext() const override {
      return _context;
    };

    const SKValue& operator[] (const SKPath& path) const override {
      for (int i = 0; i < _size; i++) {
        if (_paths[i] == path) {
          return _values[i];
        }
      }
      return SKValueNone;
    }

    virtual const SKPath& getPath(int index) const override {
      if (index < _size) {
        return _paths[index];
      }
      else {
        return SKPathInvalid;
      }
    };

    virtual const SKValue& getValue(int index) const override {
      if (index < _size) {
        return _values[index];
      }
      else {
        return SKValueNone;
      }
    };
};
