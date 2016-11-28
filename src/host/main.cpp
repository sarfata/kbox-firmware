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
//#include <SPI.h>
//#include <Encoder.h>
//#include <i2c_t3.h>
//#include <KBox.h>
//#include "util/ESPProgrammer.h"

#include <ChibiOS_ARM.h>
#include <KBox.h>
#include "SerialCommander.h"

// Those are included here to force platformio to link to them.
// See issue github.com/platformio/platformio/issues/432 for a better way to do this.
//#include <Bounce.h>

const uint8_t LED_PIN = 13;

volatile uint32_t count = 0;
ILI9341_t3 *display;

void mainThread();

//------------------------------------------------------------------------------
// thread 1 - high priority for blinking LED
// 64 byte stack beyond task switch and interrupt needs
static THD_WORKING_AREA(waThread1, 64);

static THD_FUNCTION(Thread1 ,arg) {
  pinMode(LED_PIN, OUTPUT);

  // Flash led every 200 ms.
  while (1) {
    // Turn LED on.
    digitalWrite(LED_PIN, HIGH);

    // Sleep for 50 milliseconds.
    chThdSleepMilliseconds(50);

    // Turn LED off.
    digitalWrite(LED_PIN, LOW);

    // Sleep for 150 milliseconds.
    chThdSleepMilliseconds(150);
  }
}

// FIXME: copy/paste from commander.cpp ....
struct CommandDrawPixels {
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint16_t pixels[];
};

struct CommandDrawPixels *cdp = 0;

#define NUM_COMMANDS 3
static msg_t draw_commands_queue[NUM_COMMANDS];
static mailbox_t draw_commands;

//------------------------------------------------------------------------------
static THD_WORKING_AREA(waThreadSerialCommander, 512);

static THD_FUNCTION(ThreadSerialCommander, arg) {
  SerialCommander serialCommander(Serial, 16768, display, &draw_commands);
  Serial.println("SerialCommander starting.");

  // Will never return
  serialCommander.loop();
}
//------------------------------------------------------------------------------
// thread 2 - print main thread count every second
// 100 byte stack beyond task switch and interrupt needs
static THD_WORKING_AREA(waThreadDisplay, 100);


static THD_FUNCTION(ThreadDisplay ,arg) {
  display = new ILI9341_t3(display_cs, display_dc, 255 /* rst unused */, display_mosi, display_sck, display_miso);

  analogWrite(display_backlight, BacklightIntensityMax);
  display->begin();
  display->fillScreen(ILI9341_BLUE);
  display->setRotation(display_rotation);

  DEBUG("Display thread started - time quantum=%i", CH_CFG_TIME_QUANTUM);
  while (true) {
    /* Wait until there is graphics data available */
    struct CommandDrawPixels *cdp;
    msg_t msg = chMBFetch(&draw_commands, (msg_t*)&cdp, TIME_INFINITE);

    if (msg == MSG_OK) {
      if (cdp != 0) {
        display->writeRect(cdp->x, cdp->y, cdp->width, cdp->height, cdp->pixels);
        free(cdp);
      }
    }
  }
}

//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  // wait for USB Serial
  //while (!Serial) {}

  // read any input
  //delay(2000);
  //while (Serial.read() >= 0) {}

  chBegin(mainThread);
  // chBegin never returns, main thread continues with mainThread()
  while(1) {}
}
//------------------------------------------------------------------------------
// main thread runs at NORMALPRIO
void mainThread() {
  chMBObjectInit(&draw_commands, draw_commands_queue, NUM_COMMANDS);

  chThdCreateStatic(waThread1, sizeof(waThread1),
                          NORMALPRIO + 3, Thread1, NULL);

  chThdCreateStatic(waThreadDisplay, sizeof(waThreadDisplay),
                          NORMALPRIO + 1, ThreadDisplay, NULL);

  chThdCreateStatic(waThreadSerialCommander, sizeof(waThreadSerialCommander),
                          NORMALPRIO + 2, ThreadSerialCommander, NULL);

  Serial.println("Main thread started");
  // increment counter
  while (1) {
    // must insure increment is atomic in case of context switch for print
    // should use mutex for longer critical sections
    noInterrupts();
    count++;
    interrupts();
  }
}
//------------------------------------------------------------------------------
void loop() {
 // not used
}
