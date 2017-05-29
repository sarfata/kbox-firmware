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

#ifdef __cplusplus
extern "C" {
#endif

// Defined in signalk.h
typedef struct SKContext SKContext;

/** An opaque structure used to keep the state of the JSON generator.
 */
typedef struct SKJSON SKJSON;

/** A callback function used to push data out of the generator.
 * @param s a string to be added to the output
 */
typedef void (*SKJSONPrintCallback)(const char *s);

/** Initialize the JSON generator.
 */
SKJSON *signalk_json_alloc(SKJSONPrintCallback);

/** Releases the memory used by the generator.
 */
void signalk_json_release(SKJSON *);

/** Export a SKContext content in full format.
 */
void signalk_json_export_full(SKJSON *json, SKContext *ctx);

/** Export a SKContext content in delta format.
 */
void signalk_json_export_delta(SKJSON *json, SKContext *update);

#ifdef __cplusplus
}
#endif
