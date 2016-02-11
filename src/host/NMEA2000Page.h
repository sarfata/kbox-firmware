#include <MFD.h>
#include <NMEA2000.h>
#include <N2kMessages.h>
#include <NMEA2000_teensy.h>

class NMEA2000Page : public Page {
  private:
    bool needsPainting;
    int status = -1;

    tNMEA2000_teensy NMEA2000;

  public:
    NMEA2000Page();

    void willAppear();
    bool processEvent(const TickEvent &e);
    bool processEvent(const ButtonEvent &e);
    void paint(GC &gc);
};

