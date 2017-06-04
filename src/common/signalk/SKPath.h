/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

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
  SKPathNavigationPosition,

  // Marker value - Every path below requires an index.
  SKPathEnumIndexedPaths,

  SKPathElectricalBatteries,

  SKPathInvalid
} SKPathEnum;

class SKPath {
  private:
    SKPathEnum _p;
    String _index;

  public:
    SKPath() : _p(SKPathInvalid) {};
    SKPath(SKPathEnum p) : _p(p) {
      if (p >= SKPathEnumIndexedPaths) {
        _p = SKPathInvalid;
      }
    };

    SKPath(SKPathEnum p, String index) : _p(p), _index(index) {
      if (p <= SKPathEnumIndexedPaths) {
        _p = SKPathInvalid;
        _index = String();
      }
    };

    bool operator==(const SKPath &other) const;
    bool operator!=(const SKPath &other) const;
};
