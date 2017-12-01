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

#define M_2PI 6.283185307179586

#define SKKnotToMs(x) x * 1852 / 3600
#define SKStatuteMphToMs(x) x * 1609.344 / 3600
#define SKKmphToMs(x) x * 1000 / 3600

// x * 2 * M_PI / 360
#define SKDegToRad(x) x * 0.017453292519943
// x * 360.0 / (2 * M_PI)
#define SKRadToDeg(x) x * 57.295779513082321
// x * 3600.0 / 1852.0
#define SKMsToKnot(x) x * 1.943844492440605
#define SKMsToKmh(x)  x * 3.60

// #define NORMALIZE_ANGLE(x) fmod(x, 2*M_PI)-M_PI
#define NORMALIZE_ANGLE(x) fmod( x + M_2PI, M_2PI )
