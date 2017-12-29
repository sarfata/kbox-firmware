/*
  The MIT License

  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net

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

#include "Kommand.h"

class KommandReader {
  private:
    size_t index;
    const uint8_t *buffer;
    size_t size;

  public:
    /**
     * Creates a new KommandReader to help parse the Kommand in buffer.
     *
     * The buffer will not be copied and must remain valid as long
     * as you are using the buffer!
     *
     * Note that all the readX() functions will automatically skip the 
     * KommandIdentifier.
     *
     * Size here should be the total size of the buffer. This is different
     * from the dataSize() method which returns the size of the data.
     *
     * Passing a packer with size less than 2 will cause all the functions to
     * return 0.
     */
    KommandReader(const uint8_t *buffer, size_t size);

    /**
     * Returns the KommandIdentifier for this Kommand.
     */
    uint16_t getKommandIdentifier() const;

    /**
     * Read the next byte and advance pointer by 1.
     */
    uint8_t read8();

    /**
     * Read the next two bytes and advance pointer by 2.
     */
    uint16_t read16();

    /**
     * Read the next four bytes and advance pointer by 4.
     */
    uint32_t read32();

    /**
     * Read null-terminated string and advance pointer to end of string.
     *
     * @return a null terminated string or NULL if there was no null terminated
     * string in buffer.
     */
    const char *readNullTerminatedString();

    /**
     * Gets a pointer to the data.
     */
    const uint8_t* dataBuffer() const;

    /**
     * Get the current value of the reading index.
     */
    size_t dataIndex() const;

    /**
     * Start reading from the beginning of the packet again.
     * (The first byte after the KommandIdentifier header.
     */
    void reset();

    /**
     * Returns the size of the data.
     */
    size_t dataSize() const;
};

