#include "ADCPage.h"
#include "board.h"

ADCPage::ADCPage() {
  adc.setAveraging(32);
  adc.setResolution(12);
  adc.setConversionSpeed(ADC_LOW_SPEED);
  adc.setSamplingSpeed(ADC_HIGH_SPEED);

  // If you do not force the ADC later, make sure to configure ADC1 too because
  // the ADC library might decide to use it (it round-robins read requests).
  // Also, it seems A11 and A14 (bat1 and bat3) can only be read by ADC_0
  // We could optimize reading speed by reading two adcs in parallel.
  //adc.setAveraging(32, ADC_1);
  //adc.setResolution(12, ADC_1);
  //adc.setConversionSpeed(ADC_LOW_SPEED, ADC_1);
  //adc.setSamplingSpeed(ADC_HIGH_SPEED, ADC_1);
}

void ADCPage::willAppear() {
  fetchValues();
  needsPainting = true;
  dirty = true;
}

void ADCPage::fetchValues() {
  int input_adc = adc.analogRead(input_analog, ADC_0);
  int bat1_adc = adc.analogRead(bat1_analog, ADC_0);
  int bat2_adc = adc.analogRead(bat2_analog, ADC_0);
  int bat3_adc = adc.analogRead(bat3_analog, ADC_0);

  DEBUG("ADCs: Input: %i Bat1: %i Bat2: %i Bat3: %i", input_adc, bat1_adc, bat2_adc, bat3_adc);

  input = input_adc * 21.78 / adc.getMaxValue();
  bat1 = bat1_adc * 21.78 / adc.getMaxValue();
  bat2 = bat2_adc * 21.78 / adc.getMaxValue();
  bat3 = bat3_adc * 21.78 / adc.getMaxValue();
}

bool ADCPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    fetchValues();
    needsPainting = true;
  }
  return true;
}

bool ADCPage::processEvent(const TickEvent &e) {
  static time_ms_t lastUpdate = 0;

  if (e.getMillis() > lastUpdate + 500) {
    fetchValues();
    DEBUG("ADC - Input: %f Bat1: %f Bat2: %f Bat3: %f", input, bat1, bat2, bat3);
    needsPainting = true;
  }
  return true;
}

void ADCPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  if (dirty) {
    gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
    gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  ADC Test Page");
    dirty = false;
  }

  char text[30];
  snprintf(text, sizeof(text), "12V  Input: %2.2f", input);
  gc.drawText(Point(2, 30), FontDefault, ColorWhite, text);

  snprintf(text, sizeof(text), "Bat1 Input: %2.2f", bat1);
  gc.drawText(Point(2, 60), FontDefault, ColorWhite, text);

  snprintf(text, sizeof(text), "Bat2 Input: %2.2f", bat2);
  gc.drawText(Point(2, 90), FontDefault, ColorWhite, text);

  snprintf(text, sizeof(text), "Bat3 Input: %2.2f", bat3);
  gc.drawText(Point(2, 120), FontDefault, ColorWhite, text);

  needsPainting = false;
}
