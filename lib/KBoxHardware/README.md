# KBoxHardware

KBoxHardware is part of the KBox project.

It is distributed as a library to make it easy to start a new project that uses
the KBoxHardware. Just include this library and you will have all the pins
definitions and peripherals available through the KBoxHardware object.


## Example

Very simple example that turns the display backlight off when you press the
encoder button.

    #include <KBoxHardware.h>

    void setup() {
      KBox.setup();
      KBox.setBacklight(BacklightIntensityMax);
    }

    void loop() {
      KBox.getButton.update();
      if (KBox.getButton.fallingEdge()) {
        KBox.setBacklight(BacklightIntensityOff);
      }
      if (KBox.getButton.risingEdge()) {
        KBox.setBacklight(BacklightIntensityMax);
      }
    }

## License

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
