/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

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

#include "CurrentMonitorService.h"
#include <KBoxHardware.h>
#include "common/signalk/SKUpdateStatic.h"
#include "common/signalk/SKSource.h"

void CurrentMonitorService::setup() {
  ina219.begin(ina219_address);
}

void CurrentMonitorService::loop() {
  float shuntVoltageMV = ina219.getShuntVoltage_mV();
  float busVoltage = ina219.getBusVoltage_V();


  SKUpdateStatic<4> sk;
  sk.setSource(SKSource::sourceForKBoxSensor(SKSourceInputKBoxCurrentMonitor));
  sk.setElectricalBatteriesVoltage("shunt", busVoltage);

  double current = shuntVoltageMV * 1e-3 * _config.shuntResistance;
  if (_config.shuntResistance > 0) {
    // TODO: It would be best to properly configure the INA219 properly and let it do the maths.
    sk.setElectricalBatteriesCurrent("shunt", current);
  }

  DEBUG("Current monitor: busVoltage=%.2fV shuntVoltage=%.2fmV current=%.2fA", busVoltage, shuntVoltageMV, current);
  _skHub.publish(sk);
}