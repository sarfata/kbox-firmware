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

#ifdef __cplusplus
extern "C" {
#endif

/** Every signalk operation require a pointer to a valid SKContext.
 * SKContext is an opaque struct, that is you should never modify it directly,
 * use the accessors instead.
 */
typedef struct SKContext SKContext;

/** Create a new SignalK context.
 * @param vesselName the name of the vessel
 * @param mmsi the mmsi of the vessel
 * @return an opaque SKContext object. Use signalk_release() to free the
 * memory when you are done with it.
 */
SKContext *signalk_alloc(const char *vesselName, const char *mmsi);

/** Release a SKContext object and free the associated memory.
 * This context becomes invalid afterwards and cannot be used.
 */
void signalk_release(SKContext*);

/**
 */

enum SKKey {
  SKKeyVesselName,
  SKKeyVesselMMSI,
  SKKeyNavigationPosition,
  SKKeyNavigationAttitude,
  SKKeyNavigationHeadingMagnetic,
  SKKeyNavigationMagneticVariation,

  /** this is not a real key - it's used to identify the separation between
   * key who require an instance and keys who do not require an instance.
   */
  SKKeyRequireInstanceMarker,

  // Add here the keys that require an instance to be specified
  SKKeyElectricalBatteryInstance,

  /** Maximum value for a key.
   */
  SKKeyCount
};

enum SKSourceType {
  SKSourceNMEA2000,
  SKSourceNMEA0183,
  SKSourceSignalK,
  SKSourceKBox
};

typedef enum {
  SKErrorOK,
  SKErrorInvalidArg,
  SKErrorInvalidPath,
  SKErrorUndefinedValue
} SKError;

typedef struct {
  double latitude;
  double longitude;
} SKPosition;

typedef struct {
  double roll;
  double pitch;
  double yaw;
} SKAttitude;

typedef struct {
  double voltage;
  double current;
  double temperature;
} SKDCQuantities;

typedef struct {
  SKDCQuantities dc;
  /* Missing quite a lot of the spec here. */
} SKBattery;

typedef enum {
  SKValueString,
  SKValueNumber,
  SKValuePosition,
  SKValueAttitude,
  SKValueBattery
} SKValueType;

typedef struct {
  SKValueType type;
  union {
    const char *stringValue;
    double numberValue;
    SKPosition position;
    SKAttitude attitude;
    SKBattery battery;
  } v;
} SKValue;

#define SKValueMakeString(pString) (SKValue) { .type = SKValueString, .v = { .stringValue = pString }}
#define SKValueMakeNumber(pNumber) (SKValue) { .type = SKValueNumber, .v = { .numberValue = pNumber }}
#define SKValueMakeBattery(pVoltage, pCurrent, pTemp) (SKValue) { .type = SKValueBattery, .v = { .battery = { .dc = { .voltage = pVoltage, .current = pCurrent, .temperature = pTemp } } } }

typedef struct {
  enum SKSourceType type;
  uint32_t pgn;
  const char *label;
  const char *src;
  const char* sentence;
  const char* talker;
} SKSource;

#define SKSourceMakeKBox() (SKSource) { .type = SKSourceKBox, .pgn = 0, .label = 0, .src = 0, . sentence = 0, .talker = 0 }
#define SKSourceMakeNMEA2000(pPgn, pSrc) (SKSource) { .type = SKSourceNMEA2000, .pgn = pPgn, .label = 0, .src = pSrc, .sentence = 0, .talker = 0 }

typedef struct {
  /** The MMSI of the vessel or 0 to refer to this vessel. */
  const char *vessel;
  /** The SignalK key being modified. */
  enum SKKey key;
  /** The instance of the key, or null if this key only has one instance. */
  const char *instance;
} SKPath;

#define SKPathForKey(k) (SKPath) { .vessel = 0, .key = k, .instance = 0 }
#define SKPathForKeyWithInstance(k, i) (SKPath) { .vessel = 0, .key = k, .instance = i }

/** SignalK update callback signature.
 */
typedef void (*SKUpdateCallback)(SKContext *context, const SKSource *source, const SKPath *path);

/* Sets (or clears) an update callback that will be called every time a path
 * in the SignalK model is updated.
 * @param context the signalk context
 * @param callback a callback to call for every update or null
 */
void signalk_update_set_callback(SKContext *context, SKUpdateCallback callback);

/** Initiates a new update to the SignalK model.
 * @param context the SKContext
 * @param source the source for the data in the update. This pointer and the
 * pointers in the source structure must remain valid until signalk_update_complete()
 * is called.
 */
SKError signalk_update_begin(SKContext *context, SKSource *source);

/** Signals that a SignalK update is complete. This will trigger the update
 * callback.
 */
SKError signalk_update_complete(SKContext *context);

/** Set a new value for a given key in the SignalK model.
 * @param context the signalk context
 * @param path the path of the key being updated
 * @param source the source of the update
 * @param value the new value
 */
SKError signalk_set(SKContext *context, SKPath path, SKSource *source, SKValue *value);

/** Get the value of the specified key.
 * @param context
 * @param path
 * @param[out] source
 * @param[out] value
 * @return FIXME return some error code?
 */
SKError signalk_get(SKContext *context, SKPath path, SKSource *source, SKValue *value);

/** Performs a deep-copy of path, allocating new memory to store a copy of strings that
 * path may contain.
 * Note that in the case where the path points to this vessel (vessel == 0) and there
 * is no instance specified, this will just return path.
 * @return a SKPath struct with fresh-allocated memory for strings.
 */
SKPath signalk_path_copy(SKPath path);

/**
 * Release the memory allocated by signalk_path_copy()
 */
void signalk_path_release(SKPath path);

/** Tests if a path is valid.
 * To be valid, the key must be a valid key, and instance must be specified only for a
 * key that requires it.
 * @return 0 if the path is valid or non-zero otherwise
 */
int signalk_path_is_valid(const SKPath path);

/** Performs a deep-copy of s into d, allocating new memory to store a copy of strings
 * that s may contain.
 */
void signalk_source_copy(const SKSource *s, SKSource *d);

/** Performs a deep-copy of v into d, allocating new memory to store a copy of strings
 * that v may contain.
 */
void signalk_value_copy(const SKValue *v, SKValue *d);

/**
 * Release the memory allocated by signalk_release_copy()
 *
 * Note that s itself is not free'd since it's not allocated by signalk_source_copy();
 */
void signalk_source_release(SKSource *s);

/**
 * Release the memory allocated by signalk_value_copy()
 *
 * Note that v itself is not free'd since it's not allocated by signalk_value_copy();
 */
void signalk_value_release(SKValue *v);

#ifdef __cplusplus
}
#endif
