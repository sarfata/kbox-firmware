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

#include <KBox.h>
#include "PaintCodeHelper.h"

class PaintCodePage : public BasePage {
  private:
    bool needsPainting = true;
  public:
    void paint(GC &context) {
      if (needsPainting) {
        PaintCodeContext ctx = PaintCodeContext(context);
        drawBatteryScreen3(ctx, 12.4, 2.3, 11.2, 17.8);
        needsPainting = false;
      }
    }

    void drawBatteryScreen3(PaintCodeContext& canvas, double houseVoltageValue, double houseCurrentValue, double engineVoltageValue, double solarVoltageValue, PCRect frame = {}, String resizing = "");
    void drawScalar2(PaintCodeContext& context, PCRect frame, String unitText, double value, double noValueValue, String noValueText, double lowThresholdValue, double highThresholdValue, bool isNoValueLow);
    void drawScale2(PaintCodeContext& context, double value, double noValueValue, double lowThresholdValue, double highThresholdValue, bool isNoValueLow, double scaleLowValue, String scaleLowValueCustomText, double scaleHighValue, String scaleHighValueCustomText, double scaleWidthValue, PCRect targetFrame, String resizing);

    ~PaintCodePage() {};
};
