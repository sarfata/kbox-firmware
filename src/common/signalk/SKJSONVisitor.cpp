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

#include "SKJSONVisitor.h"
#include "SKUnits.h"

void SKJSONVisitor::processSource(const SKSource &source, JsonObject &sourceObject) {
  sourceObject["label"] = _jsonBuffer.strdup(source.getLabel().c_str());

  switch (source.getInput()) {
    case SKSourceInputUnknown:
      sourceObject["type"] = "unknown";
      break;
    case SKSourceInputNMEA2000:
      sourceObject["type"] = "NMEA2000";
      sourceObject["pgn"] = source.getPGN();
      sourceObject["src"] = source.getSourceAddress();
      sourceObject["priority"] = source.getPriority();
      break;
    case SKSourceInputNMEA0183_1:
    case SKSourceInputNMEA0183_2:
      sourceObject["type"] = "NMEA0183";
      sourceObject["talker"] = _jsonBuffer.strdup(source.getTalker());
      sourceObject["sentence"] = _jsonBuffer.strdup(source.getSentence());
      break;
    case SKSourceInputSensor:
      sourceObject["type"] = "internalSensor";
      break;
    case SKSourceInputPerformance:
      sourceObject["type"] = "PerformanceCalc";
      break;
  }
}

JsonObject& SKJSONVisitor::processUpdate(const SKUpdate& update) {
  JsonObject &root = _jsonBuffer.createObject();
  root["context"] = _jsonBuffer.strdup(update.getContext().getMRN().c_str());

  JsonArray &jsonUpdates = root.createNestedArray("updates");
  JsonObject &thisUpdate = jsonUpdates.createNestedObject();

  JsonObject &source = thisUpdate.createNestedObject("source");
  processSource(update.getSource(), source);

  if (update.getTimestamp().getTime() != 0) {
    thisUpdate["timestamp"] = _jsonBuffer.strdup(update.getTimestamp().toString().c_str());
  }

  JsonArray &values = thisUpdate.createNestedArray("values");

  for (int i = 0; i < update.getSize(); i++) {
    const SKPath &p = update.getPath(i);
    const SKValue &v = update.getValue(i);

    JsonObject &obj = values.createNestedObject();
    obj["path"] = _jsonBuffer.strdup(p.toString().c_str());
    switch (v.getType()) {
      case SKValue::SKValueTypeNone:
        obj.createNestedObject("value");
        break;
      case SKValue::SKValueTypeNumber:
        obj["value"] = v.getNumberValue();
        break;
      case SKValue::SKValueTypeAttitude:
        {
          JsonObject &attitude = obj.createNestedObject("value");
          if ( v.getAttitudeValue().pitch != SKDoubleNAN)
            attitude["pitch"] = v.getAttitudeValue().pitch;
          if ( v.getAttitudeValue().roll != SKDoubleNAN)
            attitude["roll"] = v.getAttitudeValue().roll;
          if ( v.getAttitudeValue().yaw != SKDoubleNAN)
            attitude["yaw"] = v.getAttitudeValue().yaw;
        }
        break;
      case SKValue::SKValueTypePosition:
        JsonObject &position = obj.createNestedObject("value");
        position["latitude"] = v.getPositionValue().latitude;
        position["longitude"] = v.getPositionValue().longitude;
        if ( v.getPositionValue().altitude != SKDoubleNAN)
          position["altitude"] = v.getPositionValue().altitude;
        break;
    }
  }
  return root;
}
