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

typedef enum {
  SKPathNavigationCourseOverGroundTrue,
  SKPathNavigationSpeedOverGround,
  SKPathNavigationPosition
} SKPath;

class SKValue {
  private:
    SKPath _path;

    union {
      double doubleValue;
      struct {
        double latitude;
        double longitude;
      } position;
    } _value;

    // private constructor.
    // User of this class should use the static factory methods instead.
    SKValue(SKPath p) : _path(p) {};

  public:
    /**
     * Return the path of this value.
     */
    SKPath getPath() const {
      return _path;
    };

    /**
     * Create a new value for navigation.courseOverGroundTrue
     */
    static SKValue NavigationCourseOverGroundTrue(double cog) {
      SKValue v(SKPathNavigationCourseOverGroundTrue);
      v._value.doubleValue = cog;
      return v;
    };

    /**
     * Create a new value for navigation.courseSpeedOverGround
     */
    static SKValue NavigationSpeedOverGround(double sog) {
      SKValue v(SKPathNavigationSpeedOverGround);
      v._value.doubleValue = sog;
      return v;
    };

    /**
     * Create a new value for navigation.position
     */
    static SKValue NavigationPosition(double latitude, double longitude) {
      SKValue v(SKPathNavigationPosition);
      v._value.position.latitude = latitude;
      v._value.position.longitude = longitude;
      return v;
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
