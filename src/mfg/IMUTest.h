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

#include <KBox.h>
#include <Adafruit_BNO055.h>
#include "MfgTest.h"

class IMUTest : public MfgTest {
  private:
    Adafruit_BNO055 bno055;
    uint8_t sysCalib, gyroCalib, accelCalib, magCalib;
    imu::Vector<3> eulerAngles;

    bool systemOk = false;
  public:
    IMUTest(KBox& kbox) : MfgTest(kbox, "IMUTest", 180*1000) {};

    void setup() {
      if (!bno055.begin()) {
        fail("Unable to init bno055");
      }
      setInstructions("Move KBox to calibrate IMU");
    };

    void loop() {
      uint8_t system_status, self_test_status, system_error;
      bno055.getSystemStatus(&system_status, &self_test_status, &system_error);

      if (system_status != 5) {
        fail("BNO055 System status should be 5 (was " + String(system_status) + ")");
      }
      if (system_error != 0) {
        fail("BNO055 SysError " + String(system_error) + ")");
      }
      if (self_test_status != 0x0f) {
        fail("BNO055 Self test failed: " + String(self_test_status));
      }

      bno055.getCalibration(&sysCalib, &gyroCalib, &accelCalib, &magCalib);
      eulerAngles = bno055.getVector(Adafruit_BNO055::VECTOR_EULER);

      setMessage("Calib Sys: " + String(sysCalib) + " Gyro: " + String(gyroCalib) + " Accel: " + String(accelCalib) + " Mag: " + String(magCalib) +
          "\nX: " + eulerAngles.x() + " Y: " + eulerAngles.y() + " Z: " + eulerAngles.z());

      if (sysCalib == 3 && gyroCalib == 3 && accelCalib == 3 && magCalib == 3) {
        pass();
      }
    };
};
