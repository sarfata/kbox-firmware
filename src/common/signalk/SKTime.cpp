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

#include "SKTime.h"

#include <cmath>

/*
 * Time math copied from Teensy/Time.cpp - Original copyright follows.
 *
 * Copied here to use uint32_t and avoid a conflict of definition for time_t
 * which is different when compiling unit tests.
 */

/*
  time.c - low level time and date functions
  Copyright (c) Michael Margolis 2009-2014

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  1.0  6  Jan 2010 - initial release
  1.1  12 Feb 2010 - fixed leap year calculation error
  1.2  1  Nov 2010 - fixed setTime bug (thanks to Korman for this)
  1.3  24 Mar 2012 - many edits by Paul Stoffregen: fixed timeStatus() to update
                     status, updated examples for Arduino 1.0, fixed ARM
                     compatibility issues, added TimeArduinoDue and TimeTeensy3
                     examples, add error checking and messages to RTC examples,
                     add examples to DS1307RTC library.
  1.4  5  Sep 2014 - compatibility with Arduino 1.5.7
*/
// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)

typedef struct  {
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Wday;   // day of week, sunday is day 1
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;   // offset from 1970;
} tmElements_t;

static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

static void breakTime(uint32_t timeInput, tmElements_t &tm){
// break the given time_t into time components
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!

  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)timeInput;
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24; // now it is days
  tm.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1

  year = 0;
  days = 0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  tm.Year = year; // year is offset from 1970

  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0

  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
        break;
    }
  }
  tm.Month = month + 1;  // jan is month 1
  tm.Day = time + 1;     // day of month
}

uint32_t makeTime(const tmElements_t &tm){
// assemble time elements into time_t
// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9

  int i;
  uint32_t seconds;

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds= tm.Year*(SECS_PER_DAY * 365);
  for (i = 0; i < tm.Year; i++) {
    if (LEAP_YEAR(i)) {
      seconds +=  SECS_PER_DAY;   // add extra days for leap years
    }
  }

  // add days for this year, months start from 1
  for (i = 1; i < tm.Month; i++) {
    if ( (i == 2) && LEAP_YEAR(tm.Year)) {
      seconds += SECS_PER_DAY * 29;
    } else {
      seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
    }
  }
  seconds+= (tm.Day-1) * SECS_PER_DAY;
  seconds+= tm.Hour * SECS_PER_HOUR;
  seconds+= tm.Minute * SECS_PER_MIN;
  seconds+= tm.Second;
  return seconds;
}

SKTime SKTime::timeFromNMEAStrings(String dateString, String timeString) {
  tmElements_t tm;

  tm.Day = dateString.substring(0, 2).toInt();
  tm.Month = dateString.substring(2, 4).toInt();
  // Year 2070 bug incoming ...
  int year = dateString.substring(4, 6).toInt();
  if (year < 70) {
    tm.Year = year + 30;
  }
  else {
    tm.Year = year - 70;
  }
  tm.Hour = timeString.substring(0, 2).toInt();
  tm.Minute = timeString.substring(2, 4).toInt();
  tm.Second = timeString.substring(4, 6).toInt();

  uint32_t timestamp = makeTime(tm);
  uint32_t milliseconds = unknownMilliseconds;

  if (timeString.length() > 7) {
    String ms = timeString.substring(7, 10);
    int millis = ms.toInt();

    if (ms.length() == 1) {
      millis *= 100;
    }
    if (ms.length() == 2) {
      millis *= 10;
    }

    if (millis < 1000) {
      milliseconds = millis;
    }
  }

  return SKTime(timestamp, milliseconds);
}

SKTime SKTime::timeFromNMEA2000(uint16_t daysSince1970, double secondsSinceMidnight) {
  double integralPart;
  double fractionalPart = modf(secondsSinceMidnight, &integralPart);

  uint32_t ts = daysSince1970 * SECS_PER_DAY + integralPart;

  return SKTime(ts, round(fractionalPart * 1000));
}

bool SKTime::operator==(const SKTime &other) const {
  return _timestamp == other._timestamp && _milliseconds == other._milliseconds;
}

bool SKTime::operator!=(const SKTime &other) const {
  return !(*this == other);
}

SKTime& SKTime::operator+=(uint32_t ms) {
  if (!hasMilliseconds()) {
    _milliseconds = 0;
  }

  _timestamp += ms / 1000;
  // This will never overflow because both operands are less than 1000
  _milliseconds += ms % 1000;

  // Should loop once at most.
  while (_milliseconds >= 1000) {
    _timestamp += 1;
    _milliseconds -= 1000;
  }

  return *this; // return the result by reference
}

String SKTime::toString() const {
  tmElements_t tm;
  breakTime(_timestamp, tm);

  String s;
  if (!hasMilliseconds()) {
    // 2014-04-10T08:33:53Z => 20
    s.reserve(21);
  }
  else {
    // "2014-04-10T08:33:53.010Z" => 24
    s.reserve(25);
  }
  s += (1970 + tm.Year); s += "-";
  if (tm.Month < 10) { s += "0"; }
  s += tm.Month; s += "-";
  if (tm.Day < 10) { s += "0"; }
  s += tm.Day; s += "T";
  if (tm.Hour < 10) { s += "0"; }
  s += tm.Hour; s += ":";
  if (tm.Minute < 10) { s += "0"; }
  s += tm.Minute; s += ":";
  if (tm.Second < 10) { s += "0"; }
  s += tm.Second;

  if (hasMilliseconds()) {
    s += ".";
    if (_milliseconds < 100) {
      s += "0";
    }
    if (_milliseconds < 10) {
      s += "0";
    }
    s += _milliseconds;
  }
  s += "Z";
  return s;
}

String SKTime::iso8601date() const {
  String s;
  s.reserve(11);

  tmElements_t tm;
  breakTime(_timestamp, tm);
  s += (1970 + tm.Year); s += "-";
  if (tm.Month < 10) { s += "0"; }
  s += tm.Month; s += "-";
  if (tm.Day < 10) { s += "0"; }
  s += tm.Day;

  return s;
}

String SKTime::iso8601extendedTime() const {
  String s;
  s.reserve(9);

  tmElements_t tm;
  breakTime(_timestamp, tm);
  if (tm.Hour < 10) { s += "0"; }
  s += tm.Hour; s += ":";
  if (tm.Minute < 10) { s += "0"; }
  s += tm.Minute; s += ":";
  if (tm.Second < 10) { s += "0"; }
  s += tm.Second;

  return s;
}