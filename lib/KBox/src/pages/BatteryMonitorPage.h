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
#include "KMessage.h"
#include "ui/TextLayer.h"
//RES_MOD_10_28_17 indclude DEBUG
#include "KBoxDebug.h"

class BatteryMonitorPage : public Page, public KReceiver, public KVisitor {
  private:
    //RES_MOD_10_28_17 substatue baroPressure for starterVoltage
    TextLayer *houseVoltage, *houseCurrent, *baroPressure, *supplyVoltage;

    Color colorForVoltage(float v);
    String formatMeasurement(float measure, const char *unit);

    //RES_MOD_10_28_17 setup color for Pressure
    Color colorForPressure(float b);


  public:
    BatteryMonitorPage();

    void processMessage(const KMessage& message);
    void visit(const VoltageMeasurement&);
    //RES_MOD_10_28_17 add this to public
    void visit(const BarometerMeasurement&);
};
