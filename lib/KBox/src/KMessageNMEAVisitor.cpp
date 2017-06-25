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

#include "KMessageNMEAVisitor.h"
#include "util/NMEASentenceBuilder.h"
#include "util/nmea2000.h"
//RIGM added
#include "tasks/NMEA2000Task.h"
#include "KBoxDebug.h"

#define IMU_IN_NMEA2K true  //variable to determine whether IMU data sent out over N2K

void KMessageNMEAVisitor::visit(const NMEASentence& s) {
  nmeaContent += s.getSentence() + "\r\n";
}

void KMessageNMEAVisitor::visit(const BarometerMeasurement &bm) {
  // XDR is not a very well defined sentence. Can be used for lots of things
  // apparently but that is better than nothing.
  NMEASentenceBuilder sb("II", "XDR", 8);
  sb.setField(1, "P");
  sb.setField(2, bm.getPressure(), 5);
  sb.setField(3, "B");
  sb.setField(4, "Barometer");
  sb.setField(5, "T");
  sb.setField(6, bm.getTemperature());
  sb.setField(7, "C");
  sb.setField(8, "TempAir");
  nmeaContent += sb.toNMEA() + "\r\n";
}

void KMessageNMEAVisitor::visit(const VoltageMeasurement &vm) {
  NMEASentenceBuilder sb("II", "XDR", 4);
  sb.setField(1, "V");
  sb.setField(2, vm.getVoltage(), 2);
  sb.setField(3, "V");
  sb.setField(4, vm.getLabel());
  nmeaContent += sb.toNMEA() + "\r\n";
}

void KMessageNMEAVisitor::visit(const NMEA2000Message &n2km) {
  // $PCDIN,<PGN 6>,<Timestamp 8>,<src 2>,<data>*20

  const tN2kMsg& msg = n2km.getN2kMsg();
  char *s = nmea_pcdin_sentence_for_n2kmsg(msg, n2km.getReceivedMillis() / 1000);
  nmeaContent += String(s) + "\r\n";
  free(s);
}

void KMessageNMEAVisitor::visit(const IMUMessage &imu) {
    unsigned int _imuSequence = 1;
    
#ifdef IMU_IN_NMEA2K //adds IMU to N2K
    
    tN2kMsg n2kmessage;
    tN2kMsg n2kmessage2;
    _imuSequence++;
    //DEBUG("imuSequence: %i course: %.1f yaw: %.1f roll: %.1f pitch: %.1f",_imuSequence,imu.getCourse(),imu.getYaw(),imu.getRoll(),imu.getPitch());
    //values already in Radians from IMUTask
    SetN2kPGN127257(n2kmessage, _imuSequence, imu.getYaw(), imu.getPitch(), imu.getRoll()); //sequence consistent with KMessage, NMEA2000 task
    if(RadToDeg(imu.getCourse()) < 0){
        SetN2kPGN127250(n2kmessage2, _imuSequence, (imu.getCourse() - 0.261799), /* Deviation */ N2kDoubleNA, /* Variation */ N2kDoubleNA, N2khr_magnetic);
        //for some strange reason, when converting RadToDeg for N2K streaming purposes, values over 180 are off by 15 degrees.  This does not affect the heading shown on the display, only NMEA output viewed on other devices.  Needs further exploration to find the cause
    } else {
        SetN2kPGN127250(n2kmessage2, _imuSequence, imu.getCourse(), /* Deviation */ N2kDoubleNA, /* Variation */ N2kDoubleNA, N2khr_magnetic);
    }
    
    char *s = nmea_pcdin_sentence_for_n2kmsg(n2kmessage, millis() / 1000);
    //DEBUG("nmeaContent: %s", s);
    nmeaContent += String(s) + "\r\n";
    free(s);
    
    s = nmea_pcdin_sentence_for_n2kmsg(n2kmessage2, millis() / 1000);
    //DEBUG("nmeaContent: %s", s);
    nmeaContent += String(s) + "\r\n";
    free(s);
    
#else
    
  NMEASentenceBuilder sb("II", "XDR", 16);
  sb.setField(1, "A");
  sb.setField(2, imu.getYaw(), 1);
  sb.setField(3, "D");
  sb.setField(4, "Yaw");

  sb.setField(5, "A");
  sb.setField(6, imu.getPitch(), 1);
  sb.setField(7, "D");
  sb.setField(8, "Pitch");

  sb.setField(9, "A");
  sb.setField(10, imu.getRoll(), 1);
  sb.setField(11, "D");
  sb.setField(12, "Roll");

  sb.setField(13, "");
  sb.setField(14, imu.getCalibration());
  sb.setField(15, "");
  sb.setField(16, "Calibration");

  nmeaContent += sb.toNMEA() + "\r\n";

  NMEASentenceBuilder sb2("II", "HDM", 2);
  sb2.setField(1, imu.getCourse());
  sb2.setField(2, "M");

  nmeaContent += sb2.toNMEA() + "\r\n";
#endif

}

