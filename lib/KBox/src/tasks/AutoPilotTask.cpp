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
#include "../pages/NavigationPage.h"
#include "KBoxDebug.h"
#include "../drivers/board.h"
#include "KMessage.h"
#include <NMEA2000.h>
#include <N2kMessages.h>

double apCurrentHeading = 0;
double apTargetHeading = 0;
double apTargetRudderPosition = 0;

PID headingPID(&apCurrentHeading, &apTargetRudderPosition, &apTargetHeading, (double)P_Param, (double)I_Param, (double)D_Param, (int)REVERSE);
bool isSetup = false; //if false sets initial PID variables on bootup

//AutoPilotTask::AutoPilotTask() : apCurrentHeading(0), apTargetHeading(0), apTargetRudderPosition(0), headingPID(&apCurrentHeading, &apTargetRudderPostion, &apTargetHeading, (double)P_Param, (double)I_Param, (double)D_Param, (int) REVERSE){};

void AutoPilotTask::processMessage(const KMessage &message) {
    message.accept(*this);
}

void AutoPilotTask::visit(const NAVMessage &nav) {
    apCurrentHeading = nav.getCurrentHeading();
    apTargetHeading = nav.getTargetHeading();
    navMode = nav.getApMode();
    navDodgeMode = nav.getApDodgeMode();
    //DEBUG("navMode - %.d",navMode);
}

void AutoPilotTask::visit(const RUDMessage &rm) {
    apRudderSensorPosition = rm.getRawRudderAngle();
}

void AutoPilotTask::loop() {
    
  if (isSetup == false){ //tried a separate "setup" for init but could not get it to work
    sameLastDirection=true;
    apTargetRudderPosition = MAXRUDDERSWING / 2; //centred.  For programming purposes rudder is assumed to move through 66 degrees lock to lock
    headingPID.SetMode(AUTOMATIC);
    headingPID.SetOutputLimits(0.0, MAXRUDDERSWING); //output limits  0 = full starboard rudder (trailing edge of rudder to the right, bow moves to right)
    headingPID.SetSampleTime(250);
    DEBUG("Init autopilot complete");
    isSetup = true;
  }
    
  headingPID.Compute();
    
  apRudderCommandSent = apTargetRudderPosition; //by default send rudder command to equal target position, then modify it
  double testDegs = apTargetRudderPosition - apRudderCommandSent;
    
  if (abs(testDegs) > AUTOPILOTDEADZONE) {
    DEBUG("need to move the rudder");
        
    //then we move the rudder.
    //is it changing movement direction, we need to compensate for slack
    if (sameLastDirection && apTargetRudderPosition > apRudderCommandSent) {
        //same direction to stbd, no slack
        apRudderCommandSent = apTargetRudderPosition;
    } else if (sameLastDirection && apTargetRudderPosition < apRudderCommandSent) {
        //changed direction to port, subtract slack
        sameLastDirection = false;
        apRudderCommandSent = apTargetRudderPosition - AUTOPILOTSLACK;
    } else if (!sameLastDirection && apTargetRudderPosition < apRudderCommandSent) {
        //same direction to port
        apRudderCommandSent = apTargetRudderPosition;
    } else if (!sameLastDirection && apTargetRudderPosition > apRudderCommandSent) {
        //changed direction to stbd, add slack
        sameLastDirection = true;
        apRudderCommandSent = apTargetRudderPosition + AUTOPILOTSLACK;
    }
        
  }

  APMessage m(apTargetRudderPosition, apRudderCommandSent);
  sendMessage(m);

  if (navMode == true && navDodgeMode == false){
  
  //DEBUG("apRudderCommandSent - %.0f", apRudderCommandSent);
  //DEBUG("apRudderSensorPosition - %.0f",apRudderSensorPosition);
      
    if (apRudderCommandSent > apRudderSensorPosition){
        //send acuator stop command;
        DEBUG("moving rudder left");
        //send acuator left command;
    } else if (apRudderCommandSent < apRudderSensorPosition){
        //send acuator stop command;
        DEBUG("moving rudder right");
        //send acuator left command;
    } else { //if equal
        //send acuator stop command;
        DEBUG("stop rudder motion");
    }
  }
}








