#!/usr/bin/env python

import serial
import argparse
import struct
import time
import png

""" Courtesy of esptool.py - GPL """
def slip_reader(port):
    """Generator to read SLIP packets from a serial port.
    Yields one full SLIP packet at a time, raises exception on timeout or invalid data.

    Designed to avoid too many calls to serial.read(1), which can bog
    down on slow systems.
    """
    partial_packet = None
    in_escape = False
    while True:
        waiting = port.inWaiting()
        read_bytes = port.read(1 if waiting == 0 else waiting)
        if read_bytes == '':
            raise FatalError("Timed out waiting for packet %s" % ("header" if partial_packet is None else "content"))

        for b in read_bytes:
            if partial_packet is None:  # waiting for packet header
                if b == '\xc0':
                    partial_packet = ""
                else:
                    raise FatalError('Invalid head of packet (%r)' % b)
            elif in_escape:  # part-way through escape sequence
                in_escape = False
                if b == '\xdc':
                    partial_packet += '\xc0'
                elif b == '\xdd':
                    partial_packet += '\xdb'
                else:
                    raise FatalError('Invalid SLIP escape (%r%r)' % ('\xdb', b))
            elif b == '\xdb':  # start of escape sequence
                in_escape = True
            elif b == '\xc0':  # end of packet
                yield partial_packet
                partial_packet = None
            else:  # normal byte in packet
                partial_packet += b

class FatalError(RuntimeError):
    """
    Wrapper class for runtime errors that aren't caused by internal bugs, but by
    ESP8266 responses or input content.
    """
    def __init__(self, message):
        RuntimeError.__init__(self, message)

    @staticmethod
    def WithResult(message, result):
        """
        Return a fatal error object that includes the hex values of
        'result' as a string formatted argument.
        """
        return FatalError(message % ", ".join(hex(ord(x)) for x in result))

class RemoteDisplay(object):
    CMD_RESET = 1
    CMD_DRAW_PIXELS = 2

    def __init__(self, port):
        self._port = serial.Serial(port)
        self._port.baudrate = 115200
        self._slip_reader = slip_reader(self._port)

    def read(self):
        return self._slip_reader.next()

    def write(self, packet):
        """
        Write bytes to the serial port while performing SLIP escaping
        (Borrowed from esptool.py - thanks!)
        """
        buf = '\xc0' \
              + (packet.replace('\xdb','\xdb\xdd').replace('\xc0','\xdb\xdc')) \
              + '\xc0'
        self._port.write(buf)

    def command(self, command, data = ""):
        pkt = struct.pack('<HH', command, len(data)) + data
        self.write(pkt)

        try:
            resp = self.read()
            (cmd, length) = struct.unpack('<HH', resp)
            print "Received cmd={} length={} data={}".format(cmd, length, resp[4:])
        except StopIteration:
            pass
        except FatalError as e:
            print "Error :/" + e.message
            pass

    def reset(self):
        self.command(RemoteDisplay.CMD_RESET)

    def draw_pixels(self, x, y , width, height, pixels):
        """
        Draws pixels of given width and height at position x,y.
        Pixels should be an array of 32bits integers. Colors will be converted to 565.
        """

        if len(pixels) != width * height:
            raise IndexError("Received {} pixels. Expecting w*h={}*{}={}".format(len(pixels), width, height, width*height))

        # Convert pixel data in 16 bit format
        pixels = [ struct.pack('<H', RemoteDisplay.color565(p)) for p in pixels ]

        self.command(RemoteDisplay.CMD_DRAW_PIXELS, struct.pack('<HHHH', x, y, width, height) + "".join(pixels))

    def line(self, y, pixels):
        """
        Draws a line of pixels on the screen. Pixels should be no more than the width
        of the screen.
        Pixels should be an array of 32bits integers. Colors will be converted to 565.
        """

        print "Line at {} with {} pixels".format(y, len(pixels))

        self.draw_pixels(0, y, len(pixels), 1, pixels)

    def png(self, pngFile):
        """
        Opens a PNG and draws it on the screen. Note that it should be small enough to fit
        in our SLIP packet - otherwise this will fail.
        """

        (w, h, pixels, metadata) = png.Reader(pngFile).read()
 
        print "PNG w={} h={} metadata={}".format(w, h, metadata)
        x = (320 - w) / 2
        y = (240 - h) / 2

        i = 0
        for line in pixels:
            data = []
            # Convert into rgb
            while len(line) >= metadata['planes']:
                (r, g, b) = (0, 0, 0)
                if metadata['planes'] == 4:
                    (a, r, g, b) = line[:4]
                    line = line[4:]
                if metadata['planes'] == 3:
                    (r, g, b) = line[:3]
                    line = line[3:]
                data.append((r << 16) + (g << 8) + b)
            self.draw_pixels(x, y + i, w, 1, data)
            i = i + 1

    @staticmethod
    def color565(rgb):
        r = (rgb & 0xff0000) >> 16;
        g = (rgb & 0xff00) >> 8;
        b = rgb & 0xff;

        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

def demo(display):
    display.reset()

    display.line(10, [ 0xff0000 ] * 100 )
    display.line(30, [ 0xff00 ] * 100 )
    display.line(50, [ 0xff ] * 200 )
    for y in range(0, 10):
        display.line(y, [ 0xff0000 ] * 240)

    for y in range(10, 40):
        display.line(y, [ 0xff00 ] * 240)

    for y in range(60, 100):
        display.line(y, [ 0xff ] * 240)

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--port", help = "USB Serial Port connected to KBox", default = "/dev/tty.usbmodem1411")
    parser.add_argument("png")

    args = parser.parse_args()

    d = RemoteDisplay(args.port)

    # demo(d)

    d.reset()
    d.png(args.png)

if __name__ == '__main__':
    main()
