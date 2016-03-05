/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "KBox.h"
#include <i2c_t3.h>

void KBox::setup() {
  pinMode(led_pin, OUTPUT);

  // Initialize our I2C bus
  Wire1.begin();
  // BNO055 needs up to 1ms to read 6 registers
  Wire1.setTimeout(5000);
  Wire1.setOpMode(I2C_OP_MODE_IMM);

  pinMode(encoder_button, INPUT_PULLUP);

  neopixels.begin();

  neopixels.show();
  // seems useful to make sure the neopixels are cleared every boot.
  neopixels.show();

  taskManager.setup();
}

void KBox::loop() {
  mfd.loop();
  taskManager.loop();
}
