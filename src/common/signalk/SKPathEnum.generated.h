// This file was automatically generated by sk-code-generator.py
// DO NOT MODIFY! YOUR CHANGES WOULD BE OVERWRITTEN
// Look at src/common/signalk/SKPathEnum.h.tmpl instead or modify the script
// Generated on 2018-07-26 18:30:17.964584

typedef enum {
  SKPathInvalidPath,

  SKPathEnvironmentDepthBelowKeel,
  SKPathEnvironmentDepthBelowTransducer,
  SKPathEnvironmentDepthBelowSurface,
  SKPathEnvironmentDepthTransducerToKeel,
  SKPathEnvironmentDepthSurfaceToTransducer,
  SKPathEnvironmentOutsidePressure,
  SKPathEnvironmentOutsideTemperature,
  SKPathEnvironmentWindAngleApparent,
  SKPathEnvironmentWindAngleTrueGround,
  SKPathEnvironmentWindAngleTrueWater,
  SKPathEnvironmentWindDirectionTrue,
  SKPathEnvironmentWindDirectionMagnetic,
  SKPathEnvironmentWindSpeedTrue,
  SKPathEnvironmentWindSpeedOverGround,
  SKPathEnvironmentWindSpeedApparent,
  SKPathNavigationAttitude,
  SKPathNavigationCourseOverGroundTrue,
  SKPathNavigationDatetime,
  SKPathNavigationHeadingMagnetic,
  SKPathNavigationHeadingTrue,
  SKPathNavigationLog,
  SKPathNavigationMagneticVariation,
  SKPathNavigationPosition,
  SKPathNavigationSpeedOverGround,
  SKPathNavigationSpeedThroughWater,
  SKPathNavigationTripLog,
  SKPathSteeringRudderAngle,
  SKPathSteeringRudderAngleTarget,
  SKPathPerformanceLeeway,

  // Marker value - Every path below requires an index.
  SKPathEnumIndexedPaths,

  SKPathElectricalBatteriesVoltage,

} SKPathEnum;
