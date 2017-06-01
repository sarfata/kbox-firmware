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

#include <math.h>
#include "SKSource.h"
#include "SKContext.h"
#include "SKValue.h"

/**
 * Represents a SignalK update message.
 *
 * Each update is identified by one source and a list of SKValue (path/value).
 */
class SKUpdate {
  public:
    virtual ~SKUpdate() {};

    /**
     * Returns the number of values in this update (it might be less than the
     * capacity).
     */
    virtual int getSize() const = 0;

    /**
     * Returns the context of the values in this update.
     */
    virtual const SKContext& getContext() const = 0;

    /**
     * Returns the source of the values in this update.
     */
    virtual const SKSource& getSource() const = 0;

    /**
     * Return a value by index.
     *
     * @return SKValueNone if the index does not exist.
     */
    virtual const SKValue& operator[] (int index) const = 0;

    /**
     * Return a value by path, or null if the update does not have it.
     *
     * @note if there are multiple values with this path, the first one is
     * returned.
     * @return SKValueNone if the path does not exist in this update.
     */
    virtual const SKValue& operator[] (SKPath path) const = 0;

    // Convenience functions

    bool hasNavigationSpeedOverGround() const {
      return this->operator[](SKPathNavigationSpeedOverGround) != SKValueNone;
    };
    const double getNavigationSpeedOverGround() const {
      const SKValue& v = this->operator[](SKPathNavigationSpeedOverGround);
      if (v != SKValueNone) {
        return v._value.doubleValue;
      }
      else {
        return NAN;
      }
    };

    bool hasNavigationCourseOverGroundTrue() const {
      return this->operator[](SKPathNavigationCourseOverGroundTrue) != SKValueNone;
    };
    const double getNavigationCourseOverGroundTrue() const {
      const SKValue& v = this->operator[](SKPathNavigationCourseOverGroundTrue);
      if (v != SKValueNone) {
        return v._value.doubleValue;
      }
      else {
        return NAN;
      }
    };

    bool hasNavigationPosition() const {
      return this->operator[](SKPathNavigationPosition) != SKValueNone;
    };
    const double getNavigationPositionLatitude() const {
      const SKValue& v = this->operator[](SKPathNavigationPosition);
      if (v != SKValueNone) {
        return v._value.coords.latitude;
      }
      else {
        return NAN;
      }
    };
    const double getNavigationPositionLongitude() const {
      const SKValue& v = this->operator[](SKPathNavigationPosition);
      if (v != SKValueNone) {
        return v._value.coords.longitude;
      }
      else {
        return NAN;
      }
    };
};

