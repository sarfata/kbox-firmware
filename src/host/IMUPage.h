#include <MFD.h>
#include <Adafruit_BNO055.h>

class IMUPage : public Page {
  private:
    bool needsPainting;
    int status = -1;

    void fetchValues();

    uint8_t sysCalib, gyroCalib, accelCalib, magCalib;

    imu::Vector<3> eulerAngles;
    static const int UpdateInterval = 500;
    time_ms_t lastUpdate = 0;

    Adafruit_BNO055 bno055;

  public:
    IMUPage();

    void willAppear();
    bool processEvent(const ButtonEvent &e);
    bool processEvent(const TickEvent &e);
    void paint(GC &gc);
};

