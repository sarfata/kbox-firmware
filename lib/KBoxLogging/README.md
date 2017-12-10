# KBoxLogging

KBoxLogging is part of KBox but distributed as a library so that libraries used
by KBox (like i2c, nmea, etc) can use the DEBUG() macros and log with the rest
of the KBox system (logs can be configured to go to USB, network, files, etc).

## Usage

The library exposed one global KBoxLogging object which by default will not log
anything. You need to provide a `KBoxLogger` to start sending messages
somewhere. The simplest way is to send everything to a serial port as
demonstrated here:

    #include <KBoxLogging.h>

    void setup() {
      KBoxLogging.setLogger(new KBoxLoggerStream(Serial));
    }

    void loop() {
      // You can log via the KBoxLogging object directly:
      KBoxLogging.log(KBoxLoggingLevelDebug, "file.cpp", 42, "Hello %s", "world");

      // But it is really easier to use the macros:
      DEBUG("Something is happening!");
      INFO("Counting 1, 2, %i", 3);
      ERROR("stop!");
      delay(1000);
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
