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

#include <WString.h>
#include "common/algo/List.h"
#include "common/signalk/SKVisitor.generated.h"

class SKNMEAVisitor : SKVisitor {
  private:
    LinkedList<String> _sentences;

    //void visitSKEnvironmentDepthBelowKeel(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKEnvironmentDepthBelowTransducer(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKEnvironmentDepthBelowSurface(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKEnvironmentDepthTransducerToKeel(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKEnvironmentDepthSurfaceToTransducer(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    void visitSKEnvironmentWindAngleTrueWater(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKEnvironmentWaterTemperature(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    void visitSKEnvironmentOutsidePressure(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    void visitSKElectricalBatteriesVoltage(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    void visitSKNavigationAttitude(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKNavigationCourseOverGroundTrue(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    void visitSKNavigationHeadingMagnetic(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKNavigationLog(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKNavigationMagneticVariation(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKNavigationPosition(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKNavigationSpeedOverGround(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKNavigationSpeedThroughWater(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    //void visitSKNavigationTripLog(const SKUpdate &u, const SKPath &p, const SKValue &v) override;
    void visitSKSteeringRudderAngle(const SKUpdate &u, const SKPath &p, const SKValue &v) override;


  public:
    /**
     * Process a SKUpdate and add messages to the internal queue of messages.
     */
    void processUpdate(const SKUpdate& update) {
      visit(update);
    };

    /**
     * Retrieve the current list of sentences.
     */
    const LinkedList<String>& getSentences() const {
      return _sentences;
    };

    /**
     * Flush the list of sentences.
     */
    void flushSentences() {
      _sentences.clear();
    };
};
