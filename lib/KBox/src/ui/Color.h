/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

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

#include <stdint.h>
typedef uint16_t Color;

// FIXME: Color definitions change with displays. This should be made display-dependent.
// To add more color, this is helpful: 
// https://github.com/PaulStoffregen/ILI9341_t3/blob/master/ILI9341_t3.h#L87
static const Color ColorBlue = 0x001F;
static const Color ColorRed = 0xF800;
static const Color ColorGreen = 0x07E0;
static const Color ColorWhite = 0xFFFF;
static const Color ColorBlack = 0x0000;
static const Color ColorOrange = 0xFD20;
static const Color ColorLightGrey = 0x6D18;
static const Color ColorDarkGrey = 0x7BEF;

