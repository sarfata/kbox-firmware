/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

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

#include <stdint.h>

// Used to make sure the data in flash was written by KBox
static const uint32_t storageMagic = 0xB0A7F107;

// This needs to be updated when the flash storage becomes incompatible!
static const uint32_t storageVersion = 1;


class PersistentStorage {
  public:
    /* Those parameters must be saved when they change and be restored on
     * reboot to comply with NMEA2000 standard.
     */
    struct NMEA2000Parameters {
      /* This field should at least be 1 - otherwise the data has not ever
       * been saved.
       */
      uint8_t version;
      uint8_t n2kSource;
      uint8_t deviceInstance;
      uint8_t systemInstance;

      bool operator==(const struct NMEA2000Parameters& rhs) {
        return version == rhs.version &&
          n2kSource == rhs.n2kSource &&
          deviceInstance == rhs.deviceInstance &&
          systemInstance == rhs.systemInstance;
      };

      bool operator!=(const struct NMEA2000Parameters& rhs) {
        return !( *this == rhs );
      };
    };

    struct BNO055CalOffsets {
      uint16_t accel_offset_x;
      uint16_t accel_offset_y;
      uint16_t accel_offset_z;
      uint16_t gyro_offset_x;
      uint16_t gyro_offset_y;
      uint16_t gyro_offset_z;
      uint16_t mag_offset_x;
      uint16_t mag_offset_y;
      uint16_t mag_offset_z;
      uint16_t accel_radius;
      uint16_t mag_radius;
    };

    struct IMUHeelPitchOffsets {
      int offsetHeel;
      int offsetPitch;
    };

  private:
    // Addresses of elements in flash storage
    static const uint16_t magicAddress = 0;
    static const uint16_t versionAddress = 4;
    static const uint16_t nmea2000ParamsAddress = 8;
    static const uint16_t bno055CalOffsetsAddress = 20;     // 11 x 2 Byte
    static const uint16_t imuHeelPitchOffsetsAddress = 42;  //  2 x 2 Byte

    // Size of flash
    static const uint16_t storageUsed = 46;
    static const uint16_t storageSize = 2048; // Teensy 3.2
    //static const uint16_t storageSize = 4096; // Teensy 3.6

    static bool read(uint16_t address, void *dest, uint16_t size);
    static bool write(uint16_t address, const void *src, uint16_t size);

    static bool isInitialized();
    static void initialize();

    static const uint8_t nmea2000ParametersVersion = 1;

  public:
    static bool readNMEA2000Parameters(struct NMEA2000Parameters &p);
    static bool writeNMEA2000Parameters(struct NMEA2000Parameters &p);

    static bool readBNO055CalOffsets(struct BNO055CalOffsets &o);
    static bool writeBNO055CalOffsets(struct BNO055CalOffsets &o);

    static bool readImuHeelPitchOffsets(IMUHeelPitchOffsets &o);
    static bool writeImuHeelPitchOffsets(IMUHeelPitchOffsets &o);
};
