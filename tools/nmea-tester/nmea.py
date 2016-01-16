import serial
import time

port = serial.Serial(port = "/dev/ttyUSB0", baudrate = 4800, timeout=1)

while (True):
    print "."
    port.write("TEST STRING\r\n")
    print "sent"
    time.sleep(1)
