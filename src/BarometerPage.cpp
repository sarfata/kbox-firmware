#include "BarometerPage.h"
#include "i2c_t3.h"
#include "hardware/board.h"

#include "Adafruit_BMP280.h"

BarometerPage::BarometerPage() {
  if (!bmp280.begin(bmp280_address)) {
    DEBUG("Error initializing BMP280");
    status = 1;
  }
  else {
    status = 0;
  }
}

void BarometerPage::willAppear() {
  fetchValues();
  needsPainting = true;
}

void BarometerPage::fetchValues() {
  uint8_t reg;

  temperature = bmp280.readTemperature();
  pressure = bmp280.readPressure();

  DEBUG("Read temperature=%f and pressure=%f", temperature, pressure);
}

bool BarometerPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    fetchValues();
    needsPainting = true;
  }
  return true;
}

void BarometerPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
  gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  Barometer Test Page");

  char text[30];
  if (status == 0) {
    snprintf(text, sizeof(text), "Temperature: %f", temperature);
    gc.drawText(Point(0, 30), FontDefault, ColorWhite, text);
    snprintf(text, sizeof(text), "Pressure: %f", pressure);
    gc.drawText(Point(0, 60), FontDefault, ColorWhite, text);
  } else {
    snprintf(text, sizeof(text), "Error: %i", status);
    gc.drawText(Point(0, 30), FontDefault, ColorRed, text);
  }

  needsPainting = false;
}
