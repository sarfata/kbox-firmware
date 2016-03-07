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

#include "MFD.h"
#include "TaskManager.h"

/* Drivers */
#include "drivers/board.h"
#include "drivers/ili9341display.h"

/* Tasks */
#include "tasks/RunningLightTask.h"
#include "tasks/BarometerTask.h"
#include "tasks/ADCTask.h"
#include "tasks/NMEA2000Task.h"
#include "tasks/IMUTask.h"

/* Pages */


/* Other dependencies */
#include <Adafruit_NeoPixel.h>
#include <Bounce.h>

/* KBox class to represent all the hardware */
class KBox {
  private:
    Adafruit_NeoPixel neopixels = Adafruit_NeoPixel(2, neopixel_pin, NEO_GRB + NEO_KHZ800);
    ILI9341Display display = ILI9341Display();
    Encoder encoder = Encoder(encoder_a, encoder_b);
    Bounce button = Bounce(encoder_button, 10 /* ms */);
    MFD mfd = MFD(display, encoder, button);
    TaskManager taskManager = TaskManager();
  
  public:
    void setup();
    void loop();

    void addPage(Page *p) { mfd.addPage(p); };
    void addTask(Task *t) { taskManager.addTask(t); };
};
