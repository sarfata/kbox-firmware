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
#include "MfgTest.h"

class EncoderTestRotationLeft : public MfgTest {
  public:
    EncoderTestRotationLeft(KBox& kbox) : MfgTest(kbox, "EncoderTestRotationLeft", 5000) { };

    void setup() {
      MfgTest::setup();
      setInstructions("Turn encoder to the left");
      kbox.getEncoder().write(0);
    };

    void loop() {
      if (kbox.getEncoder().read() < 0) {
        pass();
      }
      if (kbox.getEncoder().read() > 0) {
        fail("Encoder reported turning to the right instead of left.");
      }
    };
};

class EncoderTestRotationRight : public MfgTest {
  public:
    EncoderTestRotationRight(KBox& kbox) : MfgTest(kbox, "EncoderTestRotationRight", 5000) {};

    void setup() {
      MfgTest::setup();
      kbox.getEncoder().write(0);
      setInstructions("Turn encoder to the right");
    };

    void loop() {
      if (kbox.getEncoder().read() > 0) {
        pass();
      }
      if (kbox.getEncoder().read() < 0) {
        fail("Encoder reported turning to the left instead of right.");
      }
    };
};

class EncoderTestClick : public MfgTest {
  public:
    EncoderTestClick(KBox &kbox) : MfgTest(kbox, "EncoderTestClick", 5000) {};

    void setup() {
      MfgTest::setup();
      setInstructions("Press encoder");
      kbox.getButton().update();
    };

    void loop() {
      if (kbox.getButton().update()) {
        pass();
      }
    };
};

