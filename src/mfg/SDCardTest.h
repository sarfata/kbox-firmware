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

#pragma once

#include <KBox.h>
#include <SdFat.h>
#include "MfgTest.h"

class SDCardTest : public MfgTest {
  private:
    SdFat *sd = 0;
    SdFile *logFile = 0;

  public:
    SDCardTest(KBox& kbox) : MfgTest(kbox, "SDCard test", 10000) {
    };

    void setup() {
      sd = new SdFat();
    };

    void loop() {
      if (!sd->begin(sdcard_cs)) {
        if (sd->card()->errorCode()) {
          return fail("Something went wrong ... SD card errorCode: " + String(sd->card()->errorCode()) 
              + " errorData: " + String(sd->card()->errorData()));
        }

        if (sd->vol()->fatType() == 0) {
          return fail("Can't find a valid FAT16/FAT32 partition. Try reformatting the card.");
        }

        if (!sd->vwd()->isOpen()) {
          return fail("Can't open root directory. Try reformatting the card.");
        }

        return fail("Unknown error while initializing card.");
      }

      uint32_t size = sd->card()->cardSize();
      if (size == 0) {
        fail("Can't figure out card size :(");
      }
      uint32_t sizeMB = 0.000512 * size + 0.5;
      setMessage("Card size: " + String(sizeMB) + "MB.");

      SdFile *file = new SdFile();
      if (!file->open("mfgtest.txt", O_CREAT | O_WRITE | O_EXCL)) {
        fail("Unable to open file for writing");
      }
      file->println("Manufacturing test");
      file->close();
      delete(file);

      file = new SdFile();
      char buffer[200];
      if (!file->open("mfgtest.txt", O_READ)) {
        fail("Unable to open file for reading");
      }
      int n = file->fgets(buffer, sizeof(buffer));
      if (n == 19) {
        pass();
      }
      else {
        fail("Read " + String(n) + " bytes instead of 19.");
      }
    };

    void teardown() {
      delete(sd);
    };
};
