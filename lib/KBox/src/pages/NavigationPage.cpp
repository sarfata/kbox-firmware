/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

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
 an object or to manually put it back on course.  Waypoint functionality will be added at a later time. RIGM
 */

#include "NavigationPage.h"
#include "MFD.h"
#include <stdio.h>
#include "KBoxDebug.h"


NavigationPage::NavigationPage() {
  static const int col1 = 5;
  static const int col2 = 160;
  static const int row1 = 20;
  static const int row2 = 40;
  static const int row3 = 70;
  static const int row4 = 152;
  static const int row5 = 182;

  addLayer(new TextLayer(Point(col1, row2), Size(20, 20), "Heading", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col2, row2), Size(20, 20), "Target Heading", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col1, row4), Size(20, 20), "Rudder Position", ColorWhite, ColorBlack, FontDefault));
  addLayer(new TextLayer(Point(col2, row4), Size(20, 20), "Rudder Command", ColorWhite, ColorBlack, FontDefault));

  apModeDisplay= new TextLayer(Point(col1, row1), Size(20, 20), apModeString, ColorBlue, ColorBlack, FontDefault);
  waypointDisplay = new TextLayer(Point(col2, row1), Size(20, 20), apWaypointString, ColorWhite, ColorBlack, FontDefault); //reserved for future use
  headingDisplay = new TextLayer(Point(col1, row3), Size(20, 20), "----", ColorWhite, ColorBlack, FontLarge);
  targetHeadingDisplay = new TextLayer(Point(col2, row3), Size(20, 20), "----", ColorWhite, ColorBlack, FontLarge);
  rudderPositionDisplay  = new TextLayer(Point(col1, row5), Size(20, 20), "----", ColorWhite, ColorBlack, FontLarge);
  rudderCommandDisplay = new TextLayer(Point(col2, row5), Size(20, 20), "----", ColorWhite, ColorBlack, FontLarge);

  addLayer(apModeDisplay);
  addLayer(waypointDisplay);
  addLayer(headingDisplay);
  addLayer(targetHeadingDisplay);
  addLayer(rudderPositionDisplay);
  addLayer(rudderCommandDisplay);
}

Color NavigationPage::colorForRudder(float r) {
  if (r == 0) {
    return ColorWhite;
  }
  if (r > 0) {
    return ColorGreen;
  }
  if (r < 0) {
    return ColorRed;
  }
  return ColorWhite;
}
Color NavigationPage::colorForCourse(float c) {
   return ColorWhite;
}

bool NavigationPage::processEvent(const ButtonEvent &be) {
    if (be.clickType == ButtonEventTypePressed) {
        this->buttonPressed = true;
        this->buttonPressedTimer = 0;
        //DEBUG("Setting buttonPressed to true");
    }
    
    if (be.clickType == ButtonEventTypeReleased && this->buttonPressed == true) {
        if (this->buttonPressedTimer < 2000) {
            // short click - return false to force the MFD to skip to the next page
            //DEBUG("short click");
            return false;
        }
        else if (this->buttonPressedTimer < 5000) { //medium click, if navMode is on, activate dodge mode
            //DEBUG("medium click");
            if (apMode == true){
                if (apDodgeMode == true){
                    apDodgeMode = false;
                    if (apHeadingMode == true){
                        apModeString = "APMode: Heading";
                    } else if (apWaypointMode == true){
                        apModeString = "APMode: Waypoint";
                    }
                    apModeDisplay->setText(apModeString);
                    apModeDisplay->setColor(ColorGreen);
                } else {
                    apDodgeMode = true;
                    apModeString = "APMode: Dodge     ";//extra spaces needed to overwrite old string if longer
                    apModeDisplay->setText(apModeString);
                    apModeDisplay->setColor(ColorRed);
                }
                return true;
            } else {
                DEBUG("apMode not active so no effect");
            }
        } else if (this->buttonPressedTimer > 5000) {  // Long click - start and stop the navMode here
            DEBUG("long click");
            if (imuCalibrated == true){
                if (apMode == false){
                    apMode = true;
                    apHeadingMode = true;
                    apModeString = "APMode: Heading       ";//extra spaces needed to overwrite old string if longer
                    apModeDisplay->setText(apModeString);
                    apModeDisplay->setColor(ColorGreen);
                } else {
                    apMode = false;
                    apDodgeMode = false;
                    apModeString = "APMode: Off           ";
                    apModeDisplay->setText(apModeString);
                    apModeDisplay->setColor(ColorBlue);
                }
            }
            return true;
        }
    }
    //DEBUG("APmode is: - %d", apMode);
    //DEBUG("APHeadingmode is: - %d", apHeadingMode);
    //DEBUG("APDodgemode is: - %d", apDodgeMode);
    return true;
}

bool NavigationPage::processEvent(const EncoderEvent &ee) {
    encoderClicks = ee.rotation;
    navEncoderClicks = navEncoderClicks + encoderClicks;
    encoderClicks = 0;
    return true;
}

String NavigationPage::formatMeasurement(float measure, const char *unit) {
  // extra spaces at the end needed to clear previous value completely
  // (we use a non-fixed width font)
  char s[10];
  snprintf(s, sizeof(s), "%.0f %s  ", measure, unit); //do not display decimals for headings
  return String(s);
}


void NavigationPage::processMessage(const KMessage &message) {
  message.accept(*this);
}

void NavigationPage::visit(const APMessage &ap) {
    navTargetRudderPosition = ap.getTargetRudderPosition();
    navRudderCommandSent = ap.getRudderCommandSent();
}

void NavigationPage::visit(const RUDMessage &rm) {
    navRudderSensorPosition = rm.getRudderDisplayAngle();
}

void NavigationPage::visit(const IMUMessage &imu) {
    if (imu.getCalibration() == 3){
        imuCalibrated = true;
    }
    if (imuCalibrated){
        if (!apMode){
            apModeString = "APMode: Off   ";
            apModeDisplay->setText(apModeString);
            apModeDisplay->setColor(ColorGreen);
        }
    } else {
        apModeString = "Calibrating   ";
        apModeDisplay->setText(apModeString);
        apModeDisplay->setColor(ColorRed);
    }
    
    rawHeading = RadToDeg(imu.getCourse());
    if (rawHeading < 0){
        navCurrentHeading = 720 + rawHeading;//adding 720 for nav math purposes
        navCurrentDisplayHeading = 360 + rawHeading;
    } else {
        navCurrentHeading = 720 + rawHeading;
        navCurrentDisplayHeading =  rawHeading;
    }
    headingDisplay->setText(formatMeasurement(navCurrentDisplayHeading, ""));
    headingDisplay->setColor(colorForCourse(navCurrentDisplayHeading));
    
    if (apMode == true && apHeadingMode == true ){
        if (isTargetSet == false){
            initialTargetHeading = navCurrentHeading;
            navTargetHeading = initialTargetHeading;
            isTargetSet = true;
        } else {
            navTargetHeading = initialTargetHeading + navEncoderClicks;
            if ((navTargetHeading - 720) < 0){
                navTargetDisplayHeading = 360 + (navTargetHeading - 720) ;
            } else {
                navTargetDisplayHeading = navTargetHeading - 720;
            }
        }
        targetHeadingDisplay->setText(formatMeasurement(navTargetDisplayHeading, ""));
        targetHeadingDisplay->setColor(colorForCourse(navTargetDisplayHeading));
        
        //following code recalculates navCurrentHeading based on "offcourse" value to avoid non-linear heading numbers across 180 degrees
        navOffCourse = navCurrentDisplayHeading - navTargetDisplayHeading;
        navOffCourse += (navOffCourse > 180) ? -360 : (navOffCourse < -180) ? 360 : 0;
        navCurrentHeading = navTargetHeading + navOffCourse;

        if (navTargetRudderPosition == 33){  // 0 = full starboard rudder (trailing edge of rudder to the right, bow moves to right
            rudderCommandForDisplay = 0;
        } else if (navTargetRudderPosition < 33){ //32 sb 1 so display 33 - command
            rudderCommandForDisplay = 33 - navTargetRudderPosition;
        } else if (navTargetRudderPosition > 33){ //34 sb -1 so display command - 32 as negative number
            rudderCommandForDisplay = (navTargetRudderPosition - 32) * -1 ; //rounding anomaly to avoid displaying -0
        }
        rudderCommandDisplay->setText(formatMeasurement(rudderCommandForDisplay, ""));
        rudderCommandDisplay->setColor(colorForRudder(rudderCommandForDisplay));
        
        if (!apDodgeMode){ //if not in dodgeMode show rudder movement indicator
            if (rudderCommandForDisplay > 0){
                if (navRudderSensorPosition < rudderCommandForDisplay){
                    rudderPositionDisplay->setText(formatMeasurement(navRudderSensorPosition, ">"));
                } else {
                    rudderPositionDisplay->setText(formatMeasurement(navRudderSensorPosition, "<"));
                }
            } else if (rudderCommandForDisplay < 0) {
                if (navRudderSensorPosition > rudderCommandForDisplay){
                    rudderPositionDisplay->setText(formatMeasurement(navRudderSensorPosition, "<"));
                } else {
                    rudderPositionDisplay->setText(formatMeasurement(navRudderSensorPosition, ">"));
                }
            } else {
                rudderPositionDisplay->setText(formatMeasurement(navRudderSensorPosition, "|"));
            }
        } else {
            rudderPositionDisplay->setText(formatMeasurement(navRudderSensorPosition, ""));
        }
        rudderPositionDisplay->setColor(colorForRudder(navRudderSensorPosition));
        
    } else if (apMode == true && apWaypointMode == true ){
        
        //reserved for future use when waypoint functionality added
    
    } else { //AP mode is false
       isTargetSet = false; //reset variables
       navEncoderClicks = 0;
        
       targetHeadingDisplay->setText("----  ");
       targetHeadingDisplay->setColor(ColorWhite);
        
        rudderCommandDisplay->setText("----  ");
        rudderCommandDisplay->setColor(ColorWhite);
        
        rudderPositionDisplay->setText(formatMeasurement(navRudderSensorPosition, ""));
        rudderPositionDisplay->setColor(colorForRudder(navRudderSensorPosition));
    }
    NAVMessage m(apMode, apHeadingMode, apWaypointMode, apDodgeMode, navCurrentHeading, navTargetHeading, courseToWaypoint);
    sendMessage(m);
}








