from machine import Pin
from neopixel import NeoPixel

pin = Pin(4, Pin.OUT)
np = NeoPixel(pin, 1)

np[0] = (0, 255, 30)
np.write()
