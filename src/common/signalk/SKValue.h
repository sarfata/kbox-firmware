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

/*
 * A position in the SignalK world.
 */
typedef struct SKTypePosition {
  SKTypePosition(double lat, double lon, double alt) :
    latitude(lat), longitude(lon), altitude(alt) {};

  double latitude;
  double longitude;
  double altitude;
} SKTypePosition;

/*
 * Attitude of the boat.
 */
typedef struct SKTypeAttitude {
  SKTypeAttitude(double roll, double pitch, double yaw) :
    roll(roll), pitch(pitch), yaw(yaw) {};

  /**
   * Vessel roll, +ve is list to starboard.
   */
  double roll;

  /**
   * Vessel pitch, +ve is bow up.
   */
  double pitch;

  /**
   * Vessel yaw, +ve is heading change to starboard.
   */
  double yaw;
} SKTypeAttitude;

/**
 * In memory representation of a SignalK value.
 *
 * A value is basically any element in the specification that includes
 * 'commonValueFields'. They are associated to a path by the SKUpdate that owns
 * them. To avoid using dynamic memory, the SKValue is implemented as a union
 * of the different data types that are valid. Dynamic memory is only used for
 * string-values and when using more rarely used attributes (like the meta
 * characteristics).
 *
 */
class SKValue {
  public:
    enum SKValueType {
      SKValueTypeNone,
      SKValueTypeNumber,
      SKValueTypePosition,
      SKValueTypeAttitude
    };

  private:
    // Type of the data in this SKValue
    enum SKValueType _type;

    // Storage of the actual value in an enum
    union value {
      value(double n) : numberValue(n) {};
      value(SKTypePosition p) : position(p) {};
      value(SKTypeAttitude a) : attitude(a) {};

      double numberValue;
      SKTypePosition position;
      SKTypeAttitude attitude;
      // need to add:
      // smallString - fits in the size of the union
      // largeString - uses dynamic memory allocation
      // datetimevalue
      // etc
    } _value;

    // TODO: We should add here
    // - timestamp
    // - $source (maybe - this can also be grouped at the SKUpdate level

  public:
    SKValue() : _type(SKValueTypeNone), _value(0) {};
    SKValue(double v) : _type(SKValueTypeNumber), _value(v) {};
    SKValue(SKTypePosition p) : _type(SKValueTypePosition), _value(p) {}
    SKValue(SKTypeAttitude a) : _type(SKValueTypeAttitude), _value(a) {}

    /**
     * Returns true if the two SKValues compared have the same value.
     */
    bool operator==(const SKValue& v) const;

    bool operator!=(const SKValue& v) const;

    double getNumberValue() const;
    SKTypePosition getPositionValue() const;
    SKTypeAttitude getAttitudeValue() const;

    enum SKValueType getType() const {
      return _type;
    };
};

extern const SKValue SKValueNone;

