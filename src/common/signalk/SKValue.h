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

class SKUpdate;

typedef enum {
  SKPathNavigationCourseOverGroundTrue,
  SKPathNavigationSpeedOverGround,
  SKPathNavigationPosition,

  SKPathInvalid
} SKPath;

class SKValue {
  private:
    SKPath _path;

    struct coords {
      coords(double lat, double lon) : latitude(lat), longitude(lon) {}

      double latitude;
      double longitude;
    };
    union value {
      value(double d) : doubleValue(d) {}
      value(struct coords c) : coords(c) {}
      double doubleValue;
      struct coords coords;
    } _value;

    // private constructor.
    // User of this class should use the static factory methods instead.
    SKValue(SKPath p) : _path(p) {};

    // SKUpdate is allowed to create SKValues
    friend SKUpdate;

    SKValue(SKPath& p, double v) : _path(p), _value(v) {};
    SKValue(SKPath& p, struct coords c) : _path(p), _value(c) {}
    SKValue() : _path(SKPathInvalid), _value(0) {};

  public:
    /**
     * Return the path of this value.
     */
    SKPath getPath() const {
      return _path;
    };

    /**
     * Returns true if the two SKValues compared have the same path and the same
     * value.
     */
    bool operator==(const SKValue& v) const;

    bool operator!=(const SKValue& v) const;

    /**
     * Create a new value for navigation.courseOverGroundTrue
     */
    static SKValue navigationCourseOverGroundTrue(double cog) {
      SKValue v(SKPathNavigationCourseOverGroundTrue);
      v._value.doubleValue = cog;
      return v;
    };

    /**
     * Create a new value for navigation.courseSpeedOverGround
     */
    static SKValue navigationSpeedOverGround(double sog) {
      SKValue v(SKPathNavigationSpeedOverGround);
      v._value.doubleValue = sog;
      return v;
    };

    /**
     * Create a new value for navigation.position
     */
    static SKValue navigationPosition(double latitude, double longitude) {
      SKValue v(SKPathNavigationPosition);
      v._value.position.latitude = latitude;
      v._value.position.longitude = longitude;
      return v;
    };

    /**
     * @private
     * You should use SKValueNone instead.
     */
    static SKValue noneValue() {
      return SKValue(SKPathInvalid);
    };

    double getNavigationCourseOverGroundTrue() const {
      return _value.doubleValue;
    };

    double getNavigationSpeedOverGround() const {
      return _value.doubleValue;
    };

    double getNavigationPositionLatitude() const {
      return _value.position.latitude;
    };

    double getNavigationPositionLongitude() const {
      return _value.position.longitude;
    };
};

extern const SKValue SKValueNone;

