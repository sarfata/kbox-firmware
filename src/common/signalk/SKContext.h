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

#include <WString.h>

/**
 * Provides the context that applies to a collection of SKUpdate.
 *
 * In most case, the context is SKContextSelf which means our own boat.
 *
 * When representing data relative to other boats, SKContext will provide a
 * unique identifier to the other boat.
 */
class SKContext {
  private:
    String _urn;

  public:
    /**
     * Create a new SKContext instance with a given URN.
     */
    SKContext(const String& urn) : _urn(urn) {};

    /**
     * @return a unique identifier for a vessel.
     */
    const String getURN() const {
      return _urn;
    };

    /**
     * Compares two SKContext objects and returns true if they represent the same
     * context.
     */
    bool operator==(const SKContext &other) const;
    bool operator!=(const SKContext &other) const;
};

extern const SKContext SKContextSelf;
