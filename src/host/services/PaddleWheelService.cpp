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

#include "PaddleWheelService.h"

#include <KBoxHardware.h>
#include <common/signalk/SKUpdateStatic.h>
#include "common/signalk/SKUnits.h"

/*
 * Standard paddle wheels are designed to generate 20000 pulses per mile.
 *
 * At 1knot, that is 5.5 pulses per second which is not enough to provide an
 * accurate speed measurement by counting pulses per second.
 *
 * Instead we average the duration of pulses between updates.
 */
static volatile uint32_t s_sumPulsesDurationUS;
static volatile uint32_t s_countPulses;

/**
 * Interrupt handler called every time a new pulse is detected.
 */
static void paddleWheelPulse() {
  static uint32_t lastPulse = UINT32_MAX;

  uint32_t now = micros();
  // This handles the initial start and the overflow case. We just ignore the
  // pulse in this case.
  if (now > lastPulse) {
    uint32_t pulseDuration = now - lastPulse;
    s_sumPulsesDurationUS += pulseDuration;
    s_countPulses++;
  }
  lastPulse = micros();
}

PaddleWheelService::PaddleWheelService(const PaddleWheelConfig& config,
                                         SKHub& hub) :
  Task("PaddleWheel"), _config(config), _hub(hub) {

}

void PaddleWheelService::setup() {
  Task::setup();

  s_sumPulsesDurationUS = 0;
  s_countPulses = 0;
  pinMode(paddle_wheel_input, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(paddle_wheel_input), paddleWheelPulse, RISING);
}


void PaddleWheelService::loop() {
  // Disable interrupt to make a copy of the variable that might be
  // manipulated by the interrupt handler while we process the loop.
  cli();
  uint32_t sumPulsesDurationUS = s_sumPulsesDurationUS;
  uint32_t countPulses = s_countPulses;
  // Reset the variables to 0 now that we have read them.
  s_sumPulsesDurationUS = 0;
  s_countPulses = 0;
  sei();

  double pulsePeriodUS = sumPulsesDurationUS / countPulses;
  double boatSpeed = 0;

  if (pulsePeriodUS > 0) {
    // calculate speed in mile / second
    boatSpeed = 1.0 /
      (_config.pulsesPerNauticalMile * pulsePeriodUS * 1e-6);
    // convert to mile / hour
    boatSpeed *= 3600;
  }

  DEBUG("Paddle wheel period: %.2f us boat speed: %.2f kt - SumPeriod: %i Count: %i",
        pulsePeriodUS, boatSpeed, sumPulsesDurationUS, countPulses);
  SKUpdateStatic<1> update;
  // TODO: Set the source properly once the WiFi PR is merged (it includes sources for sensors).
  update.setNavigationSpeedThroughWater(SKKnotToMs(boatSpeed));
  _hub.publish(update);
}
