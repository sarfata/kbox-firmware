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

#include "KBoxTest.h"
#include "KMessageNMEAVisitor.h"

TEST_CASE("Visiting a BarometerMeasurement object") {
  BarometerMeasurement bm(21.52, 1.02421);

  KMessageNMEAVisitor v;
  bm.accept(v);

  CHECK( v.toNMEA() == "$IIXDR,P,1.02421,B,Barometer,T,21.52,C,TempAir*68\r\n" );
}

TEST_CASE("Visiting a Voltage Measurement object") {
  VoltageMeasurement vm(0, "Supply", 12.42);

  KMessageNMEAVisitor v;
  vm.accept(v);

  CHECK( v.toNMEA() == "$IIXDR,V,12.42,V,Supply*56\r\n");
}

TEST_CASE("Visiting a NMEA0183 sentence message") {
  NMEASentence s("$GPRMC,003516.000,A,3751.6035,N,12228.8065,W,0.01,0.00,030416,,,D*79");

  KMessageNMEAVisitor v;
  s.accept(v);
  CHECK( v.toNMEA() == "$GPRMC,003516.000,A,3751.6035,N,12228.8065,W,0.01,0.00,030416,,,D*79\r\n" );
}

TEST_CASE("Visiting a IMU object") {
  IMUMessage m(3, 232.423, 10.122331, 29.028);

  KMessageNMEAVisitor v;
  m.accept(v);
  CHECK( v.toNMEA() == "$IIXDR,A,29.0,D,Heel,A,10.1,D,Pitch,A,232.4,D,Course,,3,,Calibration*4F\r\n" );
}

TEST_CASE("Visiting a NMEA2000 object") {
  tN2kMsg N2kMsg;

  // From n2kmessages.cpp SetN2kPGN127488
  N2kMsg.SetPGN(127488L);
  N2kMsg.Priority=3;
  N2kMsg.AddByte(0x01);
  N2kMsg.Add2ByteDouble(0x0100,0.25);
  N2kMsg.Add2ByteUDouble(0x0, 100);
  N2kMsg.AddByte(0x0);
  N2kMsg.AddByte(0xff); // Reserved
  N2kMsg.AddByte(0xff); // Reserved

  NMEA2000Message m(N2kMsg);
  KMessageNMEAVisitor v;
  m.accept(v);
  CHECK( v.toNMEA() == "$PCDIN,01F200,0000002A,0F,010004000000FFFF*25\r\n" );
}
