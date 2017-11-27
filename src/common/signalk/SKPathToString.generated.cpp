// This file was automatically generated by sk-code-generator.py
// DO NOT MODIFY! YOUR CHANGES WOULD BE OVERWRITTEN
// Look at src/common/signalk/SKPathToString.cpp.tmpl instead or modify the script
// Generated on 2017-11-26 19:10:57.165251

#include "SKPath.h"

String SKPath::toString() const {
  String path;

  switch (_p) {
    case SKPathEnvironmentDepthBelowKeel:
      path = "environment.depth.belowKeel";
      break;
    case SKPathEnvironmentDepthBelowTransducer:
      path = "environment.depth.belowTransducer";
      break;
    case SKPathEnvironmentDepthBelowSurface:
      path = "environment.depth.belowSurface";
      break;
    case SKPathEnvironmentDepthTransducerToKeel:
      path = "environment.depth.transducerToKeel";
      break;
    case SKPathEnvironmentDepthSurfaceToTransducer:
      path = "environment.depth.surfaceToTransducer";
      break;
    case SKPathEnvironmentOutsideApparentWindChillTemperature:
      path = "environment.outside.apparentWindChillTemperature";
      break;
    case SKPathEnvironmentOutsidePressure:
      path = "environment.outside.pressure";
      break;
    case SKPathEnvironmentWaterTemperature:
      path = "environment.water.temperature";
      break;
    case SKPathEnvironmentWindAngleApparent:
      path = "environment.wind.angleApparent";
      break;
    case SKPathEnvironmentWindAngleTrueGround:
      path = "environment.wind.angleTrueGround";
      break;
    case SKPathEnvironmentWindAngleTrueWater:
      path = "environment.wind.angleTrueWater";
      break;
    case SKPathEnvironmentWindDirectionTrue:
      path = "environment.wind.directionTrue";
      break;
    case SKPathEnvironmentWindDirectionMagnetic:
      path = "environment.wind.directionMagnetic";
      break;
    case SKPathEnvironmentWindSpeedTrue:
      path = "environment.wind.speedTrue";
      break;
    case SKPathEnvironmentWindSpeedOverGround:
      path = "environment.wind.speedOverGround";
      break;
    case SKPathEnvironmentWindSpeedApparent:
      path = "environment.wind.speedApparent";
      break;
    case SKPathElectricalBatteriesVoltage:
      path = "electrical.batteries." + _index + ".voltage";
      break;
    case SKPathNavigationAttitude:
      path = "navigation.attitude";
      break;
    case SKPathNavigationCourseOverGroundTrue:
      path = "navigation.courseOverGroundTrue";
      break;
    case SKPathNavigationHeadingMagnetic:
      path = "navigation.headingMagnetic";
      break;
    case SKPathNavigationLog:
      path = "navigation.log";
      break;
    case SKPathNavigationMagneticVariation:
      path = "navigation.magneticVariation";
      break;
    case SKPathNavigationPosition:
      path = "navigation.position";
      break;
    case SKPathNavigationSpeedOverGround:
      path = "navigation.speedOverGround";
      break;
    case SKPathNavigationSpeedThroughWater:
      path = "navigation.speedThroughWater";
      break;
    case SKPathNavigationTripLog:
      path = "navigation.trip.log";
      break;
    case SKPathSteeringRudderAngle:
      path = "steering.rudderAngle";
      break;

    case SKPathInvalidPath:
    case SKPathEnumIndexedPaths:
      path = "invalid";
      break;
  }

  return path;
};

