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

#include <stdlib.h>
#include <string.h>
#include "signalk.h"
#include "skkv.h"

char *strdup(const char *s);

struct SKContext {
  SKKV *head;
  SKUpdateCallback updateCallback;
};

SKContext *signalk_alloc(const char *vesselName, const char *mmsi) {
  if (vesselName == NULL && mmsi == NULL) {
    return NULL;
  }

  SKContext *ctx = malloc(sizeof(SKContext));
  memset(ctx, 0, sizeof(SKContext));

  if (vesselName) {
    signalk_set(ctx, SKPathForKey(SKKeyVesselName), & SKSourceMakeKBox(), & SKValueMakeString(vesselName));
  }
  if (mmsi) {
    signalk_set(ctx, SKPathForKey(SKKeyVesselMMSI), & SKSourceMakeKBox(), & SKValueMakeString(mmsi));
  }

  return ctx;
}

void signalk_release(SKContext *context) {
  if (context) {
    free(context);

    skkv_release(context->head);
  }
}

SKError signalk_get(SKContext *context, SKPath path, SKSource *source, SKValue *value) {
  if (context == NULL) {
    return SKErrorInvalidArg;
  }
  if (signalk_path_is_valid(path)) {
    return SKErrorInvalidPath;
  }

  return skkv_get(context->head, path, source, value);
}

SKError signalk_set(SKContext *context, SKPath path, SKSource *source, SKValue *value) {
  if (context == NULL) {
    return SKErrorInvalidArg;
  }
  if (signalk_path_is_valid(path)) {
    return SKErrorInvalidPath;
  }

  context->head = skkv_set(context->head, path, source, value);
  return SKErrorOK;
}

SKPath signalk_path_copy(SKPath path) {
  SKPath copy = path;
  if (path.vessel) {
    copy.vessel = strdup(path.vessel);
  }
  if (path.instance) {
    copy.instance = strdup(path.instance);
  }
  return copy;
}

void signalk_path_release(SKPath path) {
  if (path.vessel) {
    free((void*)path.vessel);
  }
  if (path.instance) {
    free((void*)path.instance);
  }
}

int signalk_path_is_valid(const SKPath path) {
  if (path.key < SKKeyRequireInstanceMarker && path.instance != 0) {
    return -1;
  }
  if (path.key > SKKeyRequireInstanceMarker && path.instance == 0) {
    return -1;
  }
  if (path.key >= SKKeyCount) {
    return -1;
  }
  if (path.key == SKKeyRequireInstanceMarker) {
    // This is a special marker and should not be used as a key.
    return -1;
  }
  return 0;
}

void signalk_source_copy(const SKSource *s, SKSource *d) {
  *d = *s;
  if (s->label) {
    d->label = strdup(s->label);
  }
  if (s->src) {
    d->src = strdup(s->src);
  }
  if (s->sentence) {
    d->sentence = strdup(s->sentence);
  }
  if (s->talker) {
    d->talker = strdup(s->talker);
  }
}

void signalk_source_release(SKSource *s) {
  if (s->label) {
    free((void*)s->label);
  }
  if (s->src) {
    free((void*)s->src);
  }
  if (s->sentence) {
    free((void*)s->sentence);
  }
  if (s->talker) {
    free((void*)s->talker);
  }
}

void signalk_value_copy(const SKValue *v, SKValue *d) {
  *d = *v;
  switch (v->type) {
    case SKValueString:
      d->v.stringValue = strdup(v->v.stringValue);
      break;
    case SKValueNumber:
    case SKValuePosition:
    case SKValueAttitude:
    case SKValueBattery:
      // Those type of values are made of double values and do not need to
      // be copied.
      break;
  }
}

void signalk_value_release(SKValue *v) {
  switch (v->type) {
    case SKValueString:
      free((void*)v->v.stringValue);
      break;
    case SKValueNumber:
    case SKValuePosition:
    case SKValueAttitude:
    case SKValueBattery:
      break;
  }
}

