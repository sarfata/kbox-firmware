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

/* AutoPilot functionality can exist at several levels.  In this initial iteration, the design
 focused on heading the boat on a defined course that is automatically set when AP functionality
 is turned on (referred to as "Heading" mode below.  Later on we could introduce an additional
 mode when a course to a waypoint is provided by a NMEA-connected device.  In addition, we have
 "Dodge" mode which suspends rudder actions temporarily in order to manually steer the boat around
 an object or to manually put it back on course.  
 
 The course calculation approach below is a simplified and adapted version of Robert Huitema's FreeboardPLC_v1_2 code.  RIGM
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



/*pending tuning,following is set in AutoPilotTask.h
#define P_Param  0.5 //proportional param P-gain, the gain determines how much change the OP will make due to a change in error
#define I_Param  0.0 //integral or I-gain, the the reset determines how much to change the OP over time due to the error
#define D_Param  0.0 //derivative or D-gain, the preact determines how much to change the OP due from a change in direction of the error
 */

PID headingPID(&apCurrentHeading, &apTargetRudderPosition, &apTargetHeading, (double)P_Param, (double)I_Param, (double)D_Param, (int)REVERSE);
bool isSetup = false; //if false sets initial PID variables on bootup

void AutoPilotTask::processMessage(const KMessage &message) {
    message.accept(*this);
}


void AutoPilotTask::visit(const NAVMessage &nav) {  //this function now working!!
    
        apCurrentHeading = nav.getCurrentHeading();
        apTargetHeading = nav.getTargetHeading();
        apRudderSensorPosition = nav.getRudderSensorPosition();
    
       // DEBUG("apPilotCurrentHeading - %.0f",apCurrentHeading);
}

void AutoPilotTask::loop() {
    
    if (isSetup == false){ //tried a separate "setup" for init but could not get it to work
    sameLastDirection=true;
    apTargetRudderPosition = 33; //centred.  For programming purposes rudder is assumed to move through 66 degrees lock to lock
    headingPID.SetMode(AUTOMATIC);
    headingPID.SetOutputLimits(0.0, 66.0); //output limits  0 = full starboard rudder (trailing edge of rudder to the right, bow moves to right)
    headingPID.SetSampleTime(250);
    DEBUG("Init autopilot complete");
    isSetup = true;
    }
    
    headingPID.Compute();
    
    autoPilotOffCourse = apTargetHeading - apCurrentHeading; //variable not currently used for anything
    
    autoPilotDeadZone = 5; //pending tuning on water
    autoPilotSlack = 5; //pending tuning on water
    apRudderCommandSent = apTargetRudderPosition; //by default send rudder command to equal target position, then modify it
    double testDegs = apTargetRudderPosition - apRudderCommandSent;
    
    if (abs(testDegs) > autoPilotDeadZone) {
        DEBUG("need to move the rudder");
        
        //then we move the rudder.
        //is it changing movement direction, we need to compensate for slack
        if (sameLastDirection && apTargetRudderPosition > apRudderCommandSent) {
            //same direction to stbd, no slack
            apRudderCommandSent = apTargetRudderPosition;
        }else if (sameLastDirection && apTargetRudderPosition < apRudderCommandSent) {
            //changed direction to port, subtract slack
            sameLastDirection = false;
            apRudderCommandSent = apTargetRudderPosition - autoPilotSlack;
        }else if (!sameLastDirection && apTargetRudderPosition < apRudderCommandSent) {
            //same direction to port
            apRudderCommandSent = apTargetRudderPosition;
        }else if (!sameLastDirection && apTargetRudderPosition > apRudderCommandSent) {
            //changed direction to stbd, add slack
            sameLastDirection = true;
            apRudderCommandSent = apTargetRudderPosition + autoPilotSlack;
        }
        
        
    }
    //DEBUG("navTargetRudderPosition - %.0f", navTargetRudderPosition);
    //DEBUG("navRudderCommandSent2 - %.0f", navRudderCommandSent);
    
    APMessage m("test", apTargetRudderPosition, apRudderCommandSent);
    sendMessage(m);

   
    //Add code here to actually move the rudder based on controller system used
    
}








