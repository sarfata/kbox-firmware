/*
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

#include <math.h>
#include "SKSource.h"
#include "SKContext.h"
#include "SKPath.h"
#include "SKValue.h"
#include "SKTime.h"

/**
 * Represents a SignalK update message.
 *
 * Each update is identified by one source and a list of SKValue (path/value).
 */
class SKUpdate {
  public:
    virtual ~SKUpdate() {};

    /**
     * Returns the number of values in this update (it might be less than the
     * capacity).
     */
    virtual int getSize() const = 0;

    /** Calculate the size of this update in memory.
     */
    virtual int getSizeBytes() const = 0;

    /**
     * Returns the context of the values in this update.
     */
    virtual const SKContext& getContext() const = 0;

    /**
     * Returns the source of the values in this update.
     */
    virtual const SKSource& getSource() const = 0;

    /**
     * Return the timestamp of this update.
     */
    virtual const SKTime& getTimestamp() const = 0;

    /**
     * Return path at given index or SKPathInvalid if index
     * is greater than getSize().
     */
    virtual const SKPath& getPath(int index) const = 0;

    /**
     * Return value at given index or SKValueNone if index is greater than
     * getSize()
     */
    virtual const SKValue& getValue(int index) const = 0;

    /**
     * Return a value by path, or SKValueNone if the update does not have it.
     *
     * @note if there are multiple values with this path, the first one is
     * returned.
     * @return SKValueNone if the path does not exist in this update.
     */
    virtual const SKValue& operator[] (const SKPath& path) const = 0;

    /**
     * Return true if the SKUpdate contains a value with the given path.
     */
    virtual bool hasPath(const SKPath &path) const = 0;

    /**
     * Add a new SKValue to this update. This will fail if the list is full.
     *
     * @return: true if succeeded, or false if the list is full.
     */
    virtual bool setValue(const SKPath path, const SKValue v) = 0;

    // Syntactic sugar to make working with SKUpdates easier to read and less
    // prone to type errors.

    bool hasElectricalBatteriesVoltage(String index) const {
      return hasPath(SKPath(SKPathElectricalBatteriesVoltage, index));
    };
    double getElectricalBatteriesVoltage(String index) const {
      return this->operator[](SKPath(SKPathElectricalBatteriesVoltage, index)).getNumberValue();
    };
    bool setElectricalBatteriesVoltage(String index, double voltage) {
      return setValue(SKPath(SKPathElectricalBatteriesVoltage, index), voltage);
    };

    bool hasEnvironmentOutsidePressure() const {
      return hasPath(SKPathEnvironmentOutsidePressure);
    };
    double getEnvironmentOutsidePressure() const {
      return this->operator[](SKPathEnvironmentOutsidePressure).getNumberValue();
    };
    bool setEnvironmentOutsidePressure(double p) {
      return this->setValue(SKPathEnvironmentOutsidePressure, p);
    };

    bool hasNavigationAttitude() const {
      return hasPath(SKPathNavigationAttitude);
    };
    SKTypeAttitude getNavigationAttitude() const {
      return this->operator[](SKPathNavigationAttitude).getAttitudeValue();
    };
    bool setNavigationAttitude(const SKTypeAttitude a) {
      return this->setValue(SKPathNavigationAttitude, a);
    };

    bool hasNavigationCourseOverGroundTrue() const {
      return hasPath(SKPathNavigationCourseOverGroundTrue);
    };
    double getNavigationCourseOverGroundTrue() const {
      return this->operator[](SKPathNavigationCourseOverGroundTrue).getNumberValue();
    };
    bool setNavigationCourseOverGroundTrue(double cog) {
      return setValue(SKPathNavigationCourseOverGroundTrue, SKValue(cog));
    };

    bool hasNavigationHeadingMagnetic() const {
      return hasPath(SKPathNavigationHeadingMagnetic);
    };
    double getNavigationHeadingMagnetic() const {
      return this->operator[](SKPathNavigationHeadingMagnetic).getNumberValue();
    };
    bool setNavigationHeadingMagnetic(double cog) {
      return setValue(SKPathNavigationHeadingMagnetic, SKValue(cog));
    };

    bool hasNavigationPosition() const {
      return hasPath(SKPathNavigationPosition);
    };
    SKTypePosition getNavigationPosition() const {
      return this->operator[](SKPathNavigationPosition).getPositionValue();
    };
    bool setNavigationPosition(SKTypePosition p) {
      return setValue(SKPathNavigationPosition, SKValue(p));
    };

    bool hasNavigationSpeedOverGround() const {
      return hasPath(SKPathNavigationSpeedOverGround);
    };
    double getNavigationSpeedOverGround() const {
      return this->operator[](SKPathNavigationSpeedOverGround).getNumberValue();
    };
    bool setNavigationSpeedOverGround(double sog) {
      return setValue(SKPathNavigationSpeedOverGround, SKValue(sog));
    };
};

