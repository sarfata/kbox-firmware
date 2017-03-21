
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

#include <List.h>
#include <WString.h>
#include "N2kMsg.h"
#include "util/nmea.h"

class KVisitor;
class NMEASentence;
class BarometerMeasurement;
class VoltageMeasurement;
class NMEA2000Message;
class IMUMessage;
class NAVMessage;
class APMessage;

class KMessage {
  private:
  public:
    virtual ~KMessage() {};

    virtual void accept(KVisitor& v) const {};
};


class KVisitor {
  public:
    virtual void visit(const NMEASentence &) {};
    virtual void visit(const BarometerMeasurement &) {};
    virtual void visit(const VoltageMeasurement &) {};
    virtual void visit(const NMEA2000Message &) {};
    virtual void visit(const IMUMessage &) {};
    virtual void visit(const NAVMessage &) {};
    virtual void visit(const APMessage &) {};

};

class NMEASentence : public KMessage {
  private:
    String sentence;
  public:
    NMEASentence(const char *s) : sentence(s) {};

    bool isValid() const {
      return nmea_is_valid(sentence.c_str());
    };

    const String& getSentence() const {
      return sentence;
    };

    void accept(KVisitor &v) const {
      v.visit(*this);
    };
};

class BarometerMeasurement : public KMessage {
  private:
    float temperature;
    // Stored in Pa which is the SI standard
    float pressure;

  public:
    BarometerMeasurement(float t, float p) : temperature(t), pressure(p) {};

    float getTemperature() const {
      return temperature;
    };

    /* Pressure in Bar. 1 bar = 1000 hPa */
    float getPressure() const {
      return pressure;
    };

    void accept(KVisitor &v) const {
      v.visit(*this);
    };
};

class VoltageMeasurement: public KMessage {
  private:
    int index;
    String label;
    float voltage;

  public:
    VoltageMeasurement(int index, String label, float voltage) : index(index), label(label), voltage(voltage) {};

    String getLabel() const {
      return label;
    };

    int getIndex() const {
      return index;
    };

    float getVoltage() const {
      return voltage;
    };

    void accept(KVisitor &v) const {
      v.visit(*this);
    };
};

class NMEA2000Message: public KMessage {
  private:
    tN2kMsg msg;
    unsigned long int received_ms;

  public:
    NMEA2000Message(tN2kMsg m) : NMEA2000Message(m, 0) {};
    NMEA2000Message(tN2kMsg m, uint32_t received_ms) : msg(m), received_ms(received_ms) {};

    const tN2kMsg& getN2kMsg() const {
      return msg;
    };

    uint32_t getReceivedMillis() const {
      return received_ms;
    };

    void accept(KVisitor &v) const {
      v.visit(*this);
    };
};

class IMUMessage: public KMessage {
  private:
    String label; //RIGM added, may not need it
    int calibration;
    double course, yaw, pitch, roll;

  public:
    static const int IMU_CALIBRATED = 3;

    IMUMessage(String label, int c, float course, float yaw, float pitch, float roll) : calibration(c), course(course), yaw(yaw), pitch(pitch), roll(roll)
    {};

    void accept(KVisitor &v) const {
      v.visit(*this);
    };
    
    String getLabel() const {
        return label;
    };

    int getCalibration() const {
      return calibration;
    };

    /*
     * Heading in Radians: for necessary precision need to use float, not double for radians (RIGM)
     */
    float getCourse() const {
      return course;
    };

    /*
     * Difference between vessel orientation and course over water in radians.
     */
    float getYaw() const {
      return yaw;
    };

    /*
     * Pitch in radians. Positive when bow rises.
     */
    float getPitch() const {
      return pitch;
    };

    /*
     * Roll in radians. Positive when tilted right.
     */
    double getRoll() const {
      return roll;
    };
};

class NAVMessage: public KMessage { //RIGM added for autopilot functionality
private:
    String label; //not sure we need a label
    bool apMode, apHeadingMode, apWaypointMode, apDodgeMode;
    double currentHeading, targetHeading, courseToWaypoint, rudderSensorPosition, targetRudderPosition, rudderCommandSent;
    
public:
    NAVMessage(String label, bool apMode, bool apHeadingMode, bool apWaypointMode, bool apDodgeMode, double currentHeading, double targetHeading, double courseToWaypoint, double rudderSensorPosition) :
    label(label), apMode(apMode), apHeadingMode(apHeadingMode), apWaypointMode(apWaypointMode), apDodgeMode(apDodgeMode),
    currentHeading(currentHeading),targetHeading(targetHeading),courseToWaypoint(courseToWaypoint),
    rudderSensorPosition(rudderSensorPosition){};
    
    void accept(KVisitor &v) const {
        v.visit(*this);
    };
    
    String getLabel() const {
        return label;
    };
    
    bool getApMode() const {
        return apMode;
    };
    
    bool getApHeadingMode() const {
        return apHeadingMode;
    };
    
    bool getApWaypointMode() const {
        return apWaypointMode;
    };
    
    bool getApDodgeMode() const {
        return apDodgeMode;
    };
    
    /*
     * Courses in Degrees, therefore using doubles:
     */
    double getCurrentHeading() const {
        return currentHeading;
    };
    
    double getTargetHeading() const {
        return targetHeading;
    };
    
    double getCourseToWaypoint() const {
        return courseToWaypoint;
    };
    
    double getRudderSensorPosition() const {
        return rudderSensorPosition;
    };
    
    
};

class APMessage: public KMessage {  //RIGM added for autopilot functionality
private:
    String label; //not sure we need a label
    double targetRudderPosition, rudderCommandSent;
    
public:
    APMessage(String label, double targetRudderPosition, double rudderCommandSent) :
    label(label), targetRudderPosition(targetRudderPosition),rudderCommandSent(rudderCommandSent){};
    
    void accept(KVisitor &v) const {
        v.visit(*this);
    };
    
    String getLabel() const {
        return label;
    };
    
    double getTargetRudderPosition() const {
        return targetRudderPosition;
    };
    
    double getRudderCommandSent() const {
        return rudderCommandSent;
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
