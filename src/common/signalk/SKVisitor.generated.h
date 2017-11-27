// This file was automatically generated by sk-code-generator.py
// DO NOT MODIFY! YOUR CHANGES WOULD BE OVERWRITTEN
// Look at src/common/signalk/SKVisitor.h.tmpl instead or modify the script
// Generated on 2017-11-26 19:10:57.166540

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

#include "SKUpdate.h"

/**
 * This class will automatically visit all known properties of a
 * SKUpdate and call protected methods to deal with each of them.
 *
 * This makes visiting a SKUpdate message much nicer.
 */
class SKVisitor {
  protected:
    void visit(const SKUpdate& u);

  virtual void visitSKEnvironmentDepthBelowKeel(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentDepthBelowTransducer(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentDepthBelowSurface(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentDepthTransducerToKeel(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentDepthSurfaceToTransducer(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentOutsideApparentWindChillTemperature(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentOutsidePressure(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentWaterTemperature(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentWindAngleApparent(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentWindAngleTrueGround(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentWindAngleTrueWater(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentWindDirectionTrue(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentWindDirectionMagnetic(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentWindSpeedTrue(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentWindSpeedOverGround(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKEnvironmentWindSpeedApparent(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKElectricalBatteriesVoltage(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKNavigationAttitude(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKNavigationCourseOverGroundTrue(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKNavigationHeadingMagnetic(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKNavigationLog(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKNavigationMagneticVariation(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKNavigationPosition(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKNavigationSpeedOverGround(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKNavigationSpeedThroughWater(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKNavigationTripLog(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
  virtual void visitSKSteeringRudderAngle(const SKUpdate &u, const SKPath &p, const SKValue &v) {};
};
