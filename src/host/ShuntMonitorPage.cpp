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
#include "ShuntMonitorPage.h"
#include "i2c_t3.h"
#include "board.h"

ShuntMonitorPage::ShuntMonitorPage() {
}

bool ShuntMonitorPage::readRegister(uint8_t registerAddress, uint16_t *registerValue) {
  Wire1.beginTransmission(ina219_address);
  Wire1.send(registerAddress);
  int ret = Wire1.endTransmission();
  if (ret != 0) {
    DEBUG("INA219 write req for register %x: %i", registerAddress, ret);
    status = ret;
    return false;
  }

  ret = Wire1.requestFrom(ina219_address, (size_t)2);
  if (ret == 2) {
    *registerValue = (Wire1.read() << 8) & Wire1.read();
    status = 0;
    return true;
  }
  else {
    DEBUG("INA219 read failed: %i bytes read.", ret);
    status = ret;
    return false;
  }
}

void ShuntMonitorPage::willAppear() {
  fetchValues();
  needsPainting = true;
}

void ShuntMonitorPage::fetchValues() {
  uint16_t busVoltageRegister, shuntVoltageRegister;

  DEBUG("Reading data from ina219");

  if (readRegister(shuntVoltageRegisterAddress, &shuntVoltageRegister) &&
      readRegister(busVoltageRegisterAddress, &busVoltageRegister) ) {
    shuntVoltage = (int16_t)shuntVoltageRegister;
    busVoltage = ((int16_t)busVoltageRegisterAddress) >> 3;
    DEBUG("Got new shuntVoltageRegister=%x busVoltageRegister=%x", shuntVoltageRegister, busVoltageRegister);
  }
  else {
    DEBUG("failed :(");
  }
}

bool ShuntMonitorPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    fetchValues();
    needsPainting = true;
  }
  return true;
}

void ShuntMonitorPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
  gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  INA219 Test Page");

  char text[30];
  if (status == 0) {
    snprintf(text, sizeof(text), "Bus Voltage: %i", busVoltage);
    gc.drawText(Point(0, 30), FontDefault, ColorWhite, text);
    snprintf(text, sizeof(text), "Shunt Voltage: %i", shuntVoltage);
    gc.drawText(Point(0, 60), FontDefault, ColorWhite, text);
  }
  else if (status == -1) {
    gc.drawText(Point(0, 30), FontDefault, ColorRed, "Press button to read bus and shunt voltages.");
  } else {
    snprintf(text, sizeof(text), "Error: %i", status);
    gc.drawText(Point(0, 30), FontDefault, ColorRed, text);
  }

  needsPainting = false;
}
