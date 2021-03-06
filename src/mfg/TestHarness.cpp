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

#include <Arduino.h>
#include "TestHarness.h"

#include "MfgTest.h"
#include "EncoderTest.h"
#include "NeopixelTest.h"
#include "ADCTest.h"
#include "NMEATest.h"
#include "N2KTest.h"
#include "IMUTest.h"
#include "BarometerTest.h"
#include "SDCardTest.h"
#include "ShuntTest.h"
#include "RunningLightTest.h"

void TestHarness::setup() {
  Serial.begin(115200);
  display.fillRectangle(Point(0, 0), display.getSize(), ColorBlack);
}

void TestHarness::updateStatus(Color c, const String& text) {
  static const Rect statusRect(220, 5, 100, 30);

  display.fillRectangle(statusRect.origin(), statusRect.size(), c);
  display.drawText(Point(225, 13), FontDefault, ColorWhite, c, text);
}

void TestHarness::runTest(MfgTest &t) {
  // Prepare the tests
  t.setup();

  // Send information to serial console
  Serial.println("----- " + t.getName() + " -----");
  Serial.println("  > " + t.getInstructions());

  // Display test information on screen
  display.fillRectangle(Point(0, 0), display.getSize(), ColorBlack);
  display.drawText(Point(5, 10), FontDefault, ColorWhite, ColorBlack, t.getName());
  display.drawText(Point(5, 70), FontDefault, ColorWhite, ColorBlack, t.getInstructions());

  // Show colorful rectangle
  updateStatus(ColorOrange, "RUNNING");

  String m = "";

  elapsedMillis ms;
  unsigned int displayedTimer = 0;

  while (!t.finished() && (t.getTimeout() == 0 || ms < t.getTimeout())) {
    t.loop();
    if (t.getMessage() != m) {
      m = t.getMessage();
      display.drawText(Point(5, 100), FontDefault, ColorLightGrey, ColorBlack, m);
      Serial.println("\t" + m);
    }
    if (displayedTimer != t.getTimeout() - ms) {
      displayedTimer = (t.getTimeout() - ms) / 1000;
      display.drawText(Point(280, 40), FontDefault, ColorLightGrey, ColorBlack, String(displayedTimer));
    }

    yield();
  }
  if (t.getTimeout() != 0 && ms >= t.getTimeout()) {
    t.fail("Timeout");
  }

  // Report result on serial console
  Serial.println("===== " + (t.successful() ? "SUCCESS" : "FAIL (" + t.getReason() + ")") + " =====");

  // And on screen
  if (t.successful()) {
    updateStatus(ColorGreen, "PASS");
  }
  else {
    updateStatus(ColorRed, "FAIL");
    display.drawText(Point(5, 90), FontDefault, ColorRed, ColorBlack, t.getReason());
  }

  t.teardown();
}

void TestHarness::runAllTests() {
  MfgTest* tests[] = {
    new EncoderTestClick(),
    new RunningLightTest(),
    new EncoderTestRotationLeft(), new EncoderTestRotationRight(), 
    new BarometerTest(),
    new N2KTest(),
    new NMEATest("NMEA2->1", NMEA2_SERIAL, NMEA1_SERIAL), 
    new NMEATest("NMEA1->2", NMEA1_SERIAL, NMEA2_SERIAL),
    new ADCTest(0), new ADCTest(1), new ADCTest(2), new ADCTest(3),
    new ShuntTest(),
    new NeopixelTest(0), new NeopixelTest(1),
    new SDCardTest(),
    new IMUTest()
  };

  Serial.println(">>>>> Starting KBox Manufacturing tests <<<<<");

  int pass = 0;
  int fail = 0;
  for (size_t i = 0; i < sizeof(tests) / sizeof(MfgTest*); i++) {
    MfgTest& t = *(tests[i]);

    runTest(t);
    if (t.successful()) {
      pass++;
    }
    else {
      fail++;
    }
    delay(2000);
  }

  display.fillRectangle(Point(0, 0), display.getSize(), ColorBlack);
  display.drawText(Point(40, 30), FontLarge, ColorWhite, ColorBlack, "All tests done");
  display.drawText(Point(60, 120), FontDefault, ColorGreen, ColorBlack, "Passed tests: " + String(pass));
  display.drawText(Point(60, 140), FontDefault, ColorRed, ColorBlack, "Failed tests: " + String(fail));

  Serial.println("\n\n>>>>> Ran all tests <<<<<\nPassed tests: \t" + String(pass) + "\nFailed tests: \t" + String(fail));

  for (size_t i = 0; i < sizeof(tests) / sizeof(MfgTest*); i++) {
    MfgTest& t = *(tests[i]);

    Serial.printf("%50s: %10s %s\n", t.getName().c_str(), t.successful() ? "PASS" : "FAIL", t.successful() ? "" : t.getReason().c_str());
  }
}
