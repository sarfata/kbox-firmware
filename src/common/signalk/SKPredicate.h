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

/**
 * A SKPredicate evaluates an instance of SKUpdate and returns true or false.
 */
class SKPredicate {
  public:
    virtual ~SKPredicate() {};

    virtual bool evaluate(const SKUpdate& update) const = 0;
};

/**
 * A predicate to filter updates with a specific context.
 */
class SKContextPredicate : public SKPredicate {
  private:
    const SKContext& _context;

  public:
    SKContextPredicate(const SKContext& context) : _context(context) {};
    ~SKContextPredicate() {};

    bool evaluate(const SKUpdate& update) const override {
      return _context == update.getContext();
    };
};
