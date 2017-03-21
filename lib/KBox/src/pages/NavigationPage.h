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

#include "MFD.h"
#include "../tasks/AutoPilotTask.h"
#include "../tasks/NMEA2000Task.h"
#include "KMessage.h"
#include "ui/TextLayer.h"

/*
extern double navCurrentHeading;
extern double navTargetHeading;
extern double navTargetRudderPosition;
extern double navRudderSensorPosition;
extern double navRudderCommandSent;
extern String navRudderMovementIndicator;
 */


class NavigationPage : public Page, public KReceiver, public KVisitor, public KGenerator {
  private:
    TextLayer *apModeDisplay, *waypointDisplay, *headingDisplay, *targetHeadingDisplay, *rudderPositionDisplay, *rudderCommandDisplay;

    Color colorForRudder(float r);
    Color colorForCourse(float c);
    String formatMeasurement(float measure, const char *unit);
    
    bool buttonPressed = false;
    elapsedMillis buttonPressedTimer;
    
    double navCurrentHeading;
    double navTargetHeading;
    double navRudderSensorPosition;
    double navTargetRudderPosition;
    double navRudderCommandSent;
    
  public:
    NavigationPage();

    void processMessage(const KMessage& message);
    void visit(const IMUMessage&);
    void visit(const APMessage&);
    void visit(const VoltageMeasurement&);
    bool processEvent(const ButtonEvent &be);
    bool processEvent(const EncoderEvent &ee);
};
