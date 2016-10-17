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

#include <string.h>
#include "signalk-path.h"

int signalk_path_cmp(const SKPath a, const SKPath b) {
  // Start by looking at the vessel
  if (a.vessel != 0 || b.vessel != 0) {
    // At this point, one of both might be 0 but not both.
    if (a.vessel == 0) {
      return -1;
    }
    if (b.vessel == 0) {
      return 1;
    }
    // Now compare the vesselNames
    int cmp = strcmp(a.vessel, b.vessel);
    if (cmp != 0) {
      return cmp;
    }
    // At this point we know that a.vessel == b.vessel
    // continue on with other criterias.
  }

  if (a.key > b.key) {
    return 1;
  }
  else if (a.key < b.key) {
    return -1;
  }
  else {
    // They have the same key so they both should have an instance
    // but we will check for 0 anyway.
    if (a.instance != 0 || b.instance != 0) {
      if (a.instance == 0) {
        return -1;
      }
      if (b.instance == 0) {
        return 1;
      }
      return strcmp(a.instance, b.instance);
    }
  }
  return 0;
}

