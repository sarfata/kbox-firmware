/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net
  Added by Rob McLean, March 2017

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

/* AutoPilot functionality can exist at several levels of complexity.  In this initial iteration, the design
 focuses on heading the boat on a defined course that is automatically set when AP "Heading" Mode is activated
 Later on we could introduce an additional mode when a course to a waypoint is provided by a NMEA-connected device.
 "Dodge" mode suspends rudder actions temporarily to manually steer the boat around
 an object or to manually put it back on course.

 The course calculation approach below is a simplified and adapted version of Robert Huitema's excellent FreeboardPLC_v1_2 code. RIGM
 */

#include "AutoPilotTask.h"
#include "KBoxDebug.h"
#include "../drivers/board.h"
#include "KMessage.h"
#include "../util/Angles.h"

AutoPilotTask::AutoPilotTask() : Task("AutoPilotTask"),
  headingPID(&currentHeading, &targetRudderPosition, &targetHeading, (double)P_Param, (double)I_Param, (double)D_Param, (int) REVERSE)
{

}

void AutoPilotTask::processMessage(const KMessage &message) {
  message.accept(*this);
}

void AutoPilotTask::visit(const IMUMessage &imuMessage) {
  if (imuMessage.getCalibration() == 3) {
    // Normalize target heading requested in the range 0 to 360 (but in radians)
    currentHeading = Angles::normalizeAbsoluteAngle(imuMessage.getCourse());

    // Now adjust the angle to find the lowest angular distance between our current course and the targetHeading.
    // For example, if the targetHeading is 350 and our currentHeading is 005, then we will transform it to 365.
    if (abs(targetHeading - currentHeading) > PI) {
      if (currentHeading > targetHeading) {
        currentHeading -= 2*M_PI;
      } else {
        currentHeading += 2*M_PI;
      }
    }
  }
  else {
    engaged = false;
  }
}

void AutoPilotTask::visit(const AutopilotControlMessage &controlMessage) {
  engaged = controlMessage.isEngaged();


  targetHeading = controlMessage.getTargetHeading();
}

void AutoPilotTask::visit(const RudderMessage &rudderMessage) {
  currentRuddderPosition = rudderMessage.getRudderAngle();
}

void AutoPilotTask::setup() {
  // Centered.
  targetRudderPosition = 0;

  headingPID.SetMode(AUTOMATIC);
  headingPID.SetOutputLimits( - MAXRUDDERSWING / 2, MAXRUDDERSWING / 2); //output limits  0 = full starboard rudder (trailing edge of rudder to the right, bow moves to right)
  headingPID.SetSampleTime(AUTOPILOT_SAMPLE_TIME);
  headingPID.SetControllerDirection(DIRECT);
  DEBUG("Init autopilot complete");
}

void AutoPilotTask::loop() {
  headingPID.Compute();

  // Very crude rudder control system. Could we use another PID to drive the
  // rudder controller?
  AutopilotCommand command = AutopilotCommandFree;
  if (engaged) {
    if (targetRudderPosition > currentRuddderPosition + AUTOPILOTSLACK) {
      command = AutopilotCommandStarboard;
    }
    else if (targetRudderPosition < currentRuddderPosition - AUTOPILOTSLACK) {
      command = AutopilotCommandPort;
    }
    else {
      command = AutopilotCommandBrake;
    }
  }

  const char *commands = "<>o-";
  DEBUG("Autopilot CurHeading=%.1f TargetHeading=%.1f RudderTarget=%.1f Command=%c", Angles::RadToDeg(currentHeading), Angles::RadToDeg(targetHeading), Angles::RadToDeg(targetRudderPosition), commands[command]);

  AutopilotStatusMessage m(engaged, targetHeading, targetRudderPosition, command);
  sendMessage(m);

  // TODO: Actually send the command to the pilot motor.
}
