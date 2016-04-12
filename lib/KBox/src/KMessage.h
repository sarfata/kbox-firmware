/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include <List.h>
#include <WString.h>
#include "NMEA2000.h"
#include "util/nmea.h"

enum class KMessageType { NMEASentence, BarometerMeasurement, VoltageMeasurement, NMEA2000Message, IMUMessage };

class KMessage {
  private:
    KMessageType _type;
  public:
    KMessage(KMessageType type) : _type(type) {};
    const KMessageType& getMessageType() const {
      return _type;
    };
    virtual String toString() const = 0;
    virtual String toSignalK() const = 0;
};

class NMEASentence : public KMessage {
  private:
    String sentence;
  public:
    NMEASentence(const char *s) : KMessage(KMessageType::NMEASentence), sentence(s) {};

    bool isValid() const {
      return nmea_is_valid(sentence.c_str());
    };

    String toString() const {
      return sentence;
    };

    String toSignalK() const {
      return "{ type: 'nmea', value: 'not implemented yet' }";
    };
};

class BarometerMeasurement : public KMessage {
  private:
    float temperature;
    float pressure;

  public:
    BarometerMeasurement(float t, float p) : KMessage(KMessageType::BarometerMeasurement), temperature(t), pressure(p) {};

    float getTemperature() const {
      return temperature;
    };

    float getPressure() const {
      return pressure;
    };

    String toString() const {
      String s("Current pressure is: ");
      s.append(String(pressure));
      return s;
    };

    String toSignalK() const {
      return String("{ type: 'pressure', value: 'not implemented yet'}");
    };
};

class VoltageMeasurement: public KMessage {
  private:
    int index;
    String label;
    float voltage;

  public:
    VoltageMeasurement(int index, String label, float voltage) : KMessage(KMessageType::VoltageMeasurement), index(index), label(label), voltage(voltage) {};

    String toString() const {
      String s("Voltage '");
      s.append(label);
      s.append("': ");
      s.append(voltage);
      return s;
    };

    String toSignalK() const {
      return String("{ type: 'voltage', value: 'not implemented yet'}");
    };

    int getIndex() const {
      return index;
    };

    float getVoltage() const {
      return voltage;
    };
};

class NMEA2000Message: public KMessage {
  private:
    tN2kMsg msg;

  public:
    NMEA2000Message(tN2kMsg m) : KMessage(KMessageType::NMEA2000Message), msg(m) {};

    const tN2kMsg& getN2kMsg() const {
      return msg;
    };

    String toString() const;

    String toSignalK() const {
      return String("{ type: 'n2k', value: 'n2k not implemented yet'}");
    };
};

class IMUMessage: public KMessage {
  private:
    int calibration;
    float course, pitch, heel;

  public:
    IMUMessage(int c, float course, float pitch, float heel) : KMessage(KMessageType::IMUMessage), calibration(c), course(course), pitch(pitch), heel(heel)
    {};

    String toString() const {
      char buf[100];
      snprintf(buf, sizeof(buf), "Calibration: %i Course: %.0f MAG Pitch: %.1f Heel: %.1f", calibration, course, pitch, heel);
      return String(buf);
    };

    String toSignalK() const {
      return String("{ type: 'imu', value: 'not implemented yet'}");
    };
};

class KReceiver {
  public:
    virtual void processMessage(const KMessage&) = 0;
};

class KGenerator {
  private:
    LinkedList<KReceiver*> connectedReceivers;

  public:
    void connectTo(KReceiver&);

  protected:
    void sendMessage(const KMessage &);
};
