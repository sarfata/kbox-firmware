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

#include "AnalogSensorService.h"

#include <KBoxHardware.h>
#include <common/signalk/SKUpdateStatic.h>
#include "common/signalk/SKUnits.h"

/*
 * Standard paddle wheels are designed to generate 20000 pulses per mile.
 *
 * At 1knot, that is 5.5 pulses per second which is not enough to provide an
 * accurate speed measurement by counting pulses per second.
 *
 * Instead we measure the period of the pulses and use this to calculate speed.
 */
static volatile uint32_t lastPulseDuration;


/*
 * TODO:
 * - Detect when no pulse has been detected for a while. Right now we are
 * unable to detect this.
 *
 */
/**
 * Interrupt handler called every time a new pulse is detected.
 */
static void paddleWheelPulse() {
  static uint32_t lastPulse = UINT32_MAX;

  // This handles the initial start and the overflow case. We just ignore the
  // pulse in this case.
  uint32_t now = micros();
  if (now > lastPulse) {
    lastPulseDuration = now - lastPulse;
  }
  lastPulse = micros();
}

AnalogSensorService::AnalogSensorService(const AnalogSensorConfig& config,
                                         SKHub& hub) :
  Task("AnalogSensor"), _config(config), _hub(hub) {

}

void AnalogSensorService::setup() {
  Task::setup();

  lastPulseDuration = 0;
  pinMode(paddle_wheel_input, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(paddle_wheel_input), paddleWheelPulse, RISING);
}


void AnalogSensorService::loop() {
  uint32_t pulsePeriod;
  // Disable interrupt to make a copy of the variable that might be
  // manipulated by the interrupt handler while we process the loop.
  cli();
  pulsePeriod = lastPulseDuration;
  // This will allow us to detect when no pulses are sent.
  lastPulseDuration = 0;
  sei();

  double boatSpeed = 0;

  if (pulsePeriod > 0) {
    // calculate speed in mile / second
    boatSpeed = 1.0 /
      (_config.pulsesPerNauticalMile * pulsePeriod * 1e-6);
    // convert to mile / hour
    boatSpeed *= 3600;

  }

  DEBUG("Paddle wheel period: %i us boat speed: %.2f kt", pulsePeriod,
        boatSpeed);
  SKUpdateStatic<1> update;
  update.setNavigationSpeedThroughWater(SKKnotToMs(boatSpeed));
  _hub.publish(update);
}
