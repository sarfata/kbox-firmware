
#pragma once

#include <stdint.h>

typedef uint16_t BacklightIntensity;
const BacklightIntensity BacklightIntensityMax = 255;
const BacklightIntensity BacklightIntensityOff = 0;

class Display : public GC {
  public:
    // Returns the physical size of the display in pixels
    virtual const Size& getSize() const = 0;

    // Set backlight intensity
    virtual void setBacklight(BacklightIntensity intensity);
};

