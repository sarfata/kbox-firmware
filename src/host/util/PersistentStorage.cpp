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

#include <KBoxLogging.h>
#include "PersistentStorage.h"

// Implemented in cores/teens3/eeprom.c
extern "C" {
void eeprom_write_byte(uint8_t *addr, uint8_t value);
void eeprom_read_block(void *buf, const void *addr, uint32_t len);
void eeprom_write_block(const void *buf, void *addr, uint32_t len);
}

void PersistentStorage::initialize() {
  write(magicAddress, (uint8_t*)&storageMagic, sizeof(storageMagic));
  write(versionAddress, (uint8_t*)&storageVersion, sizeof(storageVersion));

  // Clear the rest of the flash back to 0
  for (int i = 8; i < storageUsed; i++) {
    eeprom_write_byte((uint8_t*)i, 0);
  }
}

bool PersistentStorage::isInitialized() {
  uint32_t magic = 0;
  uint32_t version = 0;

  read(0, &magic, sizeof(magic));
  read(4, &version, sizeof(version));

  if (magic == storageMagic && version == storageVersion) {
    return true;
  }
  else {
    return false;
  }
}

bool PersistentStorage::read(uint16_t address, void *dest, uint16_t size) {
  if (address + size > storageSize) {
    return false;
  }

  eeprom_read_block(dest, (void*)address, size);
  return true;
}

bool PersistentStorage::write(uint16_t address, const void *src, uint16_t size) {
  if (address + size > storageSize) {
    return false;
  }

  eeprom_write_block(src, (void*)address, size);

  return true;
}

bool PersistentStorage::readNMEA2000Parameters(struct NMEA2000Parameters &p) {
  if (!isInitialized()) {
    return false;
  }

  if (!read(nmea2000ParamsAddress, &p, sizeof(struct NMEA2000Parameters))) {
    return false;
  }

  if (p.version != nmea2000ParametersVersion) {
    return false;
  }
  return true;
}

bool PersistentStorage::writeNMEA2000Parameters(struct NMEA2000Parameters &p) {
  if (!isInitialized()) {
    initialize();
  }

  p.version = nmea2000ParametersVersion;
  return write(nmea2000ParamsAddress, &p, sizeof(NMEA2000Parameters));
}

