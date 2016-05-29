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

#include <Encoder.h>
#include <Bounce.h>
#include "drivers/board.h"

#include "MfgTest.h"

class EncoderTest: public MfgTest {
  protected:
    Encoder *e;
    Bounce *button;

  public:
    EncoderTest(const String& name) : MfgTest(name, 5000) {};

    virtual void setup() {
      MfgTest::setup();
      e = new Encoder(encoder_a, encoder_b);
      pinMode(encoder_button, INPUT_PULLUP);
      button = new Bounce(encoder_button, 10);
    };

    virtual void teardown() {
      delete(e);
      delete(button);
      e = 0;
      button = 0;
    };

    virtual void loop() = 0;
};

class EncoderTestRotationLeft : public EncoderTest {
  public:
    EncoderTestRotationLeft() : EncoderTest("EncoderTestRotationLeft") {};

    void setup() {
      EncoderTest::setup();
      setInstructions("Turn encoder to the left");
    };

    void loop() {
      if (e->read() < 0) {
        pass();
      }
      if (e->read() > 0) {
        fail("Encoder reported turning to the right instead of left.");
      }
    };
};

class EncoderTestRotationRight : public EncoderTest {
  public:
    EncoderTestRotationRight() : EncoderTest("EncoderTestRotationRight") {};

    void setup() {
      EncoderTest::setup();
      setInstructions("Turn encoder to the right");
    };

    void loop() {
      if (e->read() > 0) {
        pass();
      }
      if (e->read() < 0) {
        fail("Encoder reported turning to the left instead of right.");
      }
    };
};

class EncoderTestClick : public EncoderTest {
  public:
    EncoderTestClick() : EncoderTest("EncoderTestClick") {};

    void setup() {
      EncoderTest::setup();
      setInstructions("Press encoder");
    };

    void loop() {
      if (button->update()) {
        pass();
      }
    };
};

