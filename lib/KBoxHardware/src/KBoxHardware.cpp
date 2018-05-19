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

#include <malloc.h>
#include <i2c_t3.h>
#include <KBoxLogging.h>
#include "KBoxHardware.h"

KBoxHardware KBox;

void KBoxHardware::setup() {
  pinMode(led_pin, OUTPUT);

  // Initialize our I2C bus
  Wire1.begin();
  // BNO055 needs up to 1ms to read 6 registers
  Wire1.setTimeout(5000);
  //Wire1.setOpMode(I2C_OP_MODE_IMM);
  // BNO055 only supports up to 400kHz
  Wire1.setRate(I2C_RATE_400);

  // Initialize encoder pins
  pinMode(encoder_button, INPUT_PULLUP);

  // Initialize neopixels
  neopixels.begin();
  neopixels.show();
  // seems useful to make sure the neopixels are cleared every boot.
  neopixels.show();

  // Initialize serialports
  digitalWrite(nmea1_out_enable, 0);
  digitalWrite(nmea2_out_enable, 0);
  pinMode(nmea1_out_enable, OUTPUT);
  pinMode(nmea2_out_enable, OUTPUT);
  NMEA1_SERIAL.begin(38400);
  NMEA2_SERIAL.begin(4800);

  // Initialize can transceiver pins
  digitalWrite(can_standby, 1);
  pinMode(can_standby, OUTPUT);

  // Initialize ADC
  adc.setAveraging(1);
  adc.setResolution(12);
  adc.setConversionSpeed(ADC_CONVERSION_SPEED::LOW_SPEED);
  adc.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);

#ifndef BOARD_v1_revA
  adc.setReference(ADC_REFERENCE::REF_EXT, ADC_0);
  adc.setReference(ADC_REFERENCE::REF_EXT, ADC_1);
#endif
  // If you do not force the ADC later, make sure to configure ADC1 too because
  // the ADC library might decide to use it (it round-robins read requests).
  // Also, it seems A11 and A14 (bat1 and bat3) can only be read by ADC_0
  // We could optimize reading speed by reading two adcs in parallel.
  //adc.setAveraging(32, ADC_1);
  //adc.setResolution(12, ADC_1);
  //adc.setConversionSpeed(ADC_LOW_SPEED, ADC_1);
  //adc.setSamplingSpeed(ADC_HIGH_SPEED, ADC_1);

  // We need to do this because the SdSpiAltDriver does not do it.
  SPI.setMOSI(display_mosi);
  SPI.setMISO(display_miso);
  SPI.setSCK(display_sck);

  _sdCardSuccess = sdCardInit();

  display.begin();
  display.fillScreen(ILI9341_BLUE);
  display.setRotation(display_rotation);

  setBacklight(BacklightIntensityMax);
}

void KBoxHardware::setBacklight(BacklightIntensity intensity) {
  if (display_backlight) {
    if (intensity > 0) {
      analogWrite(display_backlight, intensity);
    }
    else {
      analogWrite(display_backlight, 0);
    }
  }
}

void KBoxHardware::espInit() {
  WiFiSerial.begin(1000000);
  WiFiSerial.setTimeout(0);

  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_rst, 0);

  // wifi_program is gpio0, wifi_cs is gpio15
  // gpio2 is pulled up in hardware
  digitalWrite(wifi_program, 0);
  digitalWrite(wifi_cs, 0);

  pinMode(wifi_enable, OUTPUT);
  pinMode(wifi_rst, OUTPUT);
  pinMode(wifi_program, OUTPUT);

  if (wifi_cs > 0) {
    digitalWrite(wifi_cs, 0);
    pinMode(wifi_cs, OUTPUT);
  }

}

void KBoxHardware::espRebootInFlasher() {
  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_rst, 0);
  digitalWrite(wifi_program, 0);

  delay(10);

  // Boot the ESP module
  digitalWrite(wifi_enable, 1);
  digitalWrite(wifi_rst, 1);
}

void KBoxHardware::espRebootInProgram() {
  digitalWrite(wifi_enable, 0);
  digitalWrite(wifi_rst, 0);
  digitalWrite(wifi_program, 1);

  delay(10);

  // Boot the ESP module
  digitalWrite(wifi_enable, 1);
  digitalWrite(wifi_rst, 1);
}

bool KBoxHardware::sdCardInit() {
  #if defined(__MK66FX1M0__)
    // SDIO support for Builtin SD-Card in Teensy 3.6
    if (!_sd.begin()){
  #else
    // KBox, Teensy 3.2
    if (!_sd.begin(sdcard_cs)){
  #endif
    if (_sd.card()->errorCode()) {
      DEBUG("Something went wrong ... SD card errorCode: 0x%x errorData: 0x%x", _sd.card()->errorCode(),
            _sd.card()->errorData());
      return false;
    }

    if (_sd.vol()->fatType() == 0) {
      DEBUG("Can't find a valid FAT16/FAT32 partition. Try reformatting the card.");
      return false;
    }

    if (!_sd.vwd()->isOpen()) {
      DEBUG("Can't open root directory. Try reformatting the card.");
      return false;
    }

    DEBUG("Unknown error while initializing card.");
    return false;
  }
  else {
    DEBUG("SdCard successfully initialized.");
  }

  uint32_t size = _sd.card()->cardSize();
  if (size == 0) {
    DEBUG("Can't figure out card size :(");
    return false;
  }
  uint32_t sizeMB = 0.000512 * size + 0.5;
  DEBUG("Card size: %luMB. Volume is FAT%i Cluster size: %i bytes", sizeMB, _sd.vol()->fatType(), 512 * _sd.vol()->blocksPerCluster());

  if ((sizeMB > 1100 && _sd.vol()->blocksPerCluster() < 64)
      || (sizeMB < 2200 && _sd.vol()->fatType() == 32)) {
    DEBUG("This card should be reformatted for best performance.");
    DEBUG("Use a cluster size of 32 KB for cards larger than 1 GB.");
    DEBUG("Only cards larger than 2 GB should be formatted FAT32.");
  }

  return true;
}

void KBoxHardware::rebootKBox() {
  // https://forum.pjrc.com/threads/24304-_reboot_Teensyduino()
  // -vs-_restart_Teensyduino()
  uint32_t* const cpuRestartAddress = (uint32_t*)0xE000ED0C;
  static const uint32_t cpuRestartValue = 0x5FA0004;

  *cpuRestartAddress = cpuRestartValue;
}

void KBoxHardware::readKBoxSerialNumber(tKBoxSerialNumber &sn) {
  sn.dwords[0] = SIM_UIDH;
  sn.dwords[1] = SIM_UIDMH;
  sn.dwords[2] = SIM_UIDML;
  sn.dwords[3] = SIM_UIDL;
}

// https://forum.pjrc.com/threads/25676-Teensy-3-how-to-know-RAM-usage
// http://man7.org/linux/man-pages/man3/mallinfo.3.html
int KBoxHardware::getFreeRam() {
  return mallinfo().arena - mallinfo().keepcost;
}

int KBoxHardware::getUsedRam() {
  return mallinfo().uordblks;
}

// see https://bigdanzblog.wordpress.com/2017/10/27/watch-dog-timer-wdt-for-teensy-3-1-and-3-2/
void KBoxHardware::watchdogSetup() {
  noInterrupts();
  // unlock access to WDOG registers
  WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;
  WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
  // Need to wait a bit..
  delayMicroseconds(1);

  // use 1 second WDT timeout
  WDOG_TOVALH = 0x006d;
  WDOG_TOVALL = 0xdd00;

  // This sets prescale clock so that the watchdog timer ticks at 7.2MHz
  WDOG_PRESC  = 0x400;

  // Set options to enable WDT. You must always do this as a SINGLE write to WDOG_CTRLH
  WDOG_STCTRLH |= WDOG_STCTRLH_ALLOWUPDATE |
                  WDOG_STCTRLH_WDOGEN | WDOG_STCTRLH_WAITEN |
                  WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_CLKSRC |
                  WDOG_STCTRLH_IRQRSTEN;
  interrupts();

  NVIC_ENABLE_IRQ(IRQ_WDOG);
}

void KBoxHardware::watchdogRefresh() {
  noInterrupts();
  WDOG_REFRESH = 0xA602;
  WDOG_REFRESH = 0xB480;
  interrupts();
}

String KBoxHardware::rebootReason() {
  if (RCM_SRS1 & RCM_SRS1_SACKERR)
    return "Stop Mode Acknowledge Error Reset";
  if (RCM_SRS1 & RCM_SRS1_MDM_AP)
    return "MDM-AP Reset";
  if (RCM_SRS1 & RCM_SRS1_SW)
    return "Software Reset";                   // reboot with SCB_AIRCR = 0x05FA0004
  if (RCM_SRS1 & RCM_SRS1_LOCKUP)
    return "Core Lockup Event Reset";
  if (RCM_SRS0 & RCM_SRS0_POR)
    return "Power-on Reset";                   // removed / applied power
  if (RCM_SRS0 & RCM_SRS0_PIN)
    return "External Pin Reset";               // Reboot with software download
  if (RCM_SRS0 & RCM_SRS0_WDOG)
    return "Watchdog(COP) Reset";              // WDT timed out
  if (RCM_SRS0 & RCM_SRS0_LOC)
    return "Loss of External Clock Reset";
  if (RCM_SRS0 & RCM_SRS0_LOL)
    return "Loss of Lock in PLL Reset";
  if (RCM_SRS0 & RCM_SRS0_LVD)
    return "Low-voltage Detect Reset";
  return "unknwon reset";
}

void watchdog_isr() {
  KBox.getDisplay().fillScreen(ILI9341_RED);
  delay(3000);
}