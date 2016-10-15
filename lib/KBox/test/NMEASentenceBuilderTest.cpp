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
#include "util/NMEASentenceBuilder.h"

TEST_CASE("generating an empty sentence") {
  NMEASentenceBuilder sb("GG", "ABC", 0);
  REQUIRE( sb.toNMEA() == "$GGABC*40" );
}

TEST_CASE("generating a sentence with one string and one float") {
  NMEASentenceBuilder sb("GG", "ABC", 2);
  sb.setField(1, 1.03403303, 5);
  sb.setField(2, "toto");

  REQUIRE( sb.toNMEA() == "$GGABC,1.03403,toto*6B" );
}

TEST_CASE("test a real nmea sentence to make sure the checksum is properly generated") {
  NMEASentenceBuilder sb("GP", "RMC", 12);

  sb.setField(1, "003516.000");
  sb.setField(2, "A");
  sb.setField(3, "3751.6035");
  sb.setField(4, "N");
  sb.setField(5, "12228.8065");
  sb.setField(6, "W");
  sb.setField(7, "0.01");
  sb.setField(8, "0.00");
  sb.setField(9, "030416");
  sb.setField(10, "");
  sb.setField(11, "");
  sb.setField(12, "D");

  REQUIRE( sb.toNMEA() == "$GPRMC,003516.000,A,3751.6035,N,12228.8065,W,0.01,0.00,030416,,,D*79" );
}
