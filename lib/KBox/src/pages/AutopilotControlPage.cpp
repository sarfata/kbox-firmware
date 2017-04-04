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

#include <stdio.h>
#include <math.h>
#include <N2kMessages.h> // DegToRad & RadToDeg
#include "MFD.h"
#include "KBoxDebug.h"
#include "AutopilotControlPage.h"

AutopilotControlPage::AutopilotControlPage() {
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

  apModeDisplay= new TextLayer(Point(col1, row1), Size(20, 20), "", ColorBlue, ColorBlack, FontDefault);
  currentHeadingDisplay = new TextLayer(Point(col1, row3), Size(20, 20), "----", ColorWhite, ColorBlack, FontLarge);
  targetHeadingDisplay = new TextLayer(Point(col2, row3), Size(20, 20), "----", ColorWhite, ColorBlack, FontLarge);
  rudderPositionDisplay  = new TextLayer(Point(col1, row5), Size(20, 20), "----", ColorWhite, ColorBlack, FontLarge);
  rudderCommandDisplay = new TextLayer(Point(col2, row5), Size(20, 20), "----", ColorWhite, ColorBlack, FontLarge);

  addLayer(apModeDisplay);
  addLayer(currentHeadingDisplay);
  addLayer(targetHeadingDisplay);
  addLayer(rudderPositionDisplay);
  addLayer(rudderCommandDisplay);
}

Color AutopilotControlPage::colorForRudder(float r) {
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

/**
 * Returns an angle between -PI (excluded) and +PI (included)
 */
double AutopilotControlPage::normalizeRelativeAngle(double angle) {
  if (!isfinite(angle)) {
    return angle;
  }
  while (angle <= -PI) {
    angle += 2*PI;
  }
  while (angle > PI) {
    angle -= 2*PI;
  }
  return angle;
}

/**
 * Returns an angle between 0 (included) and 2*PI (excluded)
 */
double AutopilotControlPage::normalizeAbsoluteAngle(double angle) {
  if (!isfinite(angle)) {
    return angle;
  }
  while (angle < 0) {
    angle += 2*PI;
  }
  while (angle >= 2*PI) {
    angle -= 2*PI;
  }
  return angle;
}

/**
 * Formats a given absolute angle (in radian) into a string between 0 and 359.
 *
 * @param isMagnetic true if the angle is a magnetic angle
 * @return a string with the angle and a unit indicator ("T" or "M")
 */
String AutopilotControlPage::formatAbsoluteAngle(double angle, bool isMagnetic) {
  angle = normalizeAbsoluteAngle(angle);

  char s[10];
  snprintf(s, sizeof(s), "%3.0f %c  ", RadToDeg(angle), isMagnetic ? 'M' : 'T');
  return String(s);
}

/**
 * Formats a given relative angle (in radian) into a string between -179 and 180.
 *
 * @return a string with the angle
 */
String AutopilotControlPage::formatRelativeAngle(double angle) {
  angle = normalizeRelativeAngle(angle);

  char s[10];
  snprintf(s, sizeof(s), "%3.0f   ", RadToDeg(angle));
  return String(s);
}

bool AutopilotControlPage::processEvent(const ButtonEvent &be) {
  if (be.clickType == ButtonEventTypePressed) {
    buttonPressed = true;
    buttonPressedTimer = 0;
  }

  if (be.clickType == ButtonEventTypeReleased && this->buttonPressed == true) {
    buttonPressed = false;

    // Short click
    if (this->buttonPressedTimer < 2000) {
      // return false to force the MFD to skip to the next page
      return false;
    }
    // Long click
    else {
      // Disable autopilot
      if (autopilotEngaged) {
        autopilotEngaged = false;
      }
      // Engage autopilot - only if IMU is calibrated
      else if (imuCalibrated) {
        autopilotEngaged = true;
      }
      // Transmit new command to autopilot task immediately
      AutopilotControlMessage m(autopilotEngaged, targetHeading);
      sendMessage(m);
      updateDisplay();
    }
  }
  return true;
}

bool AutopilotControlPage::processEvent(const EncoderEvent &ee) {
  targetHeading = normalizeAbsoluteAngle(targetHeading + DegToRad(ee.rotation));

  AutopilotControlMessage m(autopilotEngaged, targetHeading);
  sendMessage(m);
  updateDisplay();

  return true;
}

void AutopilotControlPage::processMessage(const KMessage &message) {
  message.accept(*this);
}

void AutopilotControlPage::visit(const AutopilotStatusMessage &ap) {
  autopilotEngaged = ap.isEngaged();
  targetRudderPosition = ap.getTargetRudderPosition();
  targetHeading = ap.getTargetHeading();
  autopilotCommand = ap.getCommand();
  updateDisplay();
}

void AutopilotControlPage::visit(const RudderMessage &rm) {
  currentRuddderPosition = rm.getRudderAngle();
  updateDisplay();
}

void AutopilotControlPage::visit(const IMUMessage &imu) {
  if (imu.getCalibration() == 3){
    imuCalibrated = true;
    currentHeading = imu.getCourse();
  }
  else {
    imuCalibrated = false;
  }
  updateDisplay();
}

void AutopilotControlPage::updateDisplay() {
  if (!imuCalibrated) {
    apModeDisplay->setText("Calibrating   ");
    apModeDisplay->setColor(ColorRed);
  }
  else {
    if (autopilotEngaged) {
      apModeDisplay->setText("APMode: Heading");
      apModeDisplay->setColor(ColorGreen);
    }
    else {
      apModeDisplay->setText("APMode: Off      ");
      apModeDisplay->setColor(ColorBlue);
    }
  }

  currentHeadingDisplay->setText(formatAbsoluteAngle(currentHeading, true));
  targetHeadingDisplay->setText(formatAbsoluteAngle(targetHeading, true));
  rudderPositionDisplay->setText(formatRelativeAngle(currentRuddderPosition));

  if (autopilotEngaged) {
    String commandString;
    switch (autopilotCommand) {
      case AutopilotCommandPort:
        commandString = "<";
        break;
      case AutopilotCommandStarboard:
        commandString = ">";
        break;
      case AutopilotCommandBrake:
        commandString = "o";
        break;
      case AutopilotCommandFree:
        commandString = " ";
        break;
    }
    rudderCommandDisplay->setText(formatRelativeAngle(targetRudderPosition) + " " + commandString + " ");
    rudderCommandDisplay->setColor(colorForRudder(targetRudderPosition));
  }
  else {
    rudderCommandDisplay->setText("---");
    rudderCommandDisplay->setColor(ColorWhite);
  }
}

/*
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
  }
}
*/
