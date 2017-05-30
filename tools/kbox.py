#!/usr/bin/env python
# Copyright Thomas Sarlandie - 2017
# MIT License like the rest of KBox

import serial
import argparse
import struct
import time
import png
import timeit

""" Courtesy of esptool.py - GPL """
def slip_reader(port):
    """Generator to read SLIP packets from a serial port.
    Yields one full SLIP packet at a time, raises exception on timeout or invalid data.
    Designed to avoid too many calls to serial.read(1), which can bog
    down on slow systems.
    """
    partial_packet = None
    in_escape = False
    connected = False

    while True:
        waiting = port.inWaiting()
        read_bytes = port.read(1 if waiting == 0 else waiting)
        if read_bytes == '':
            raise FatalError("Timed out waiting for packet %s" % ("header" if partial_packet is None else "content"))

        for b in read_bytes:
            if partial_packet is None:  # waiting for packet header
                if b == '\xc0':
                    partial_packet = ""
                    connected = True
                else:
                    if connected:
                        raise FatalError('Invalid head of packet (%r)' % b)
                    else:
                        # Ignore errors until we are connected (aka seen one valid packet)
                        pass

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
    invalid KBox responses or input content.
    """
    def __init__(self, message):
        RuntimeError.__init__(self, message)

    @staticmethod
    def WithFrame(message, result):
        """
        Return a fatal error object that includes the hex values of
        'result' as a string formatted argument.
        """
        return FatalError(message % ", ".join(hex(ord(x)) for x in result))

class KBox(object):
    KommandPing = 0x00
    KommandPong = 0x01
    KommandErr = 0x0F
    KommandLog = 0x10
    KommandScreenshot = 0x30
    KommandScreenshotData = 0x31

    def __init__(self, port, debug = False):
        self._port = serial.Serial(port)
        self._port.baudrate = 1000000
        self._slip_reader = slip_reader(self._port)
        self._width = 320
        self._height = 240
        self._mtu = 15000;
        self._debug = debug

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

    def readCommand(self, command):
        """
        Reads incoming frame until a frame with specified command is received.
        """

        while True:
            frame = self.read()

            if self._debug:
                print "> ({}) {}".format(len(frame), " ".join("{:02x}".format(ord(c)) for c in frame))

            if len(frame) < 2:
                raise FatalError.WithFrame("Frame is too short to be valid (%s)", frame)

            (cmd,) = struct.unpack('<H', frame[0:2])

            if cmd == command:
                return frame[2:]
            elif cmd == KBox.KommandErr:
                raise FatalError("KBox responded with an error")
            elif cmd == KBox.KommandLog:
                # Keep printing log messages while waiting
                self.printLog(frame[2:])
            else:
                print("Got unexpected command with id {}".format(cmd))

    def parseLogCommand(self, data):
        logLevels = { 0: "hD", 1: "hI", 2: "hE", 3: "wD", 4: "wI", 5: "wE" }
        (level, lineno, fnamelen) = struct.unpack('<HHH', data[0:6])
        fname = data[6:6+fnamelen]
        log = data[6+fnamelen:]

        return (logLevels[level], fname, lineno, log)

    def printLog(self, data):
        (loglevel, fname, lineno, log) = self.parseLogCommand(data)

        # remove path in name for clarity
        if "/" in fname:
            fname = fname.split("/")[-1]

        print("> {} {}:{} {}".format(loglevel, fname, lineno, log))


    def ping(self, pingId):
        print("PING[{}]".format(pingId))
        self.command(KBox.KommandPing, struct.pack('<I', pingId))
        t0 = time.time()

        ponged = False
        while not ponged:
            resp = self.readCommand(KBox.KommandPong)
            if len(resp) != 4:
                raise FatalError("KBox ponged with wrong size frame ({} instead of 4)".format(len(resp)))
            else:
                (data,) = struct.unpack('<I', resp)
                print("PONG[{}] in {} ms".format(data, (time.time() - t0) * 1000))
                ponged = True

    def command(self, command, data = ""):
        print("Sending cmd {} len {}".format(command, len(data)))
        t0 = time.time()
        pkt = struct.pack('<H', command) + data
        self.write(pkt)

    def captureScreen(self, startY = 0):
        """
        Sends a command to capture screen from line startY. The number of lines
        returned is decided by the KBox.

        Returns a tuple (firstLineIndex, pixelData)
        """
        self.command(KBox.KommandScreenshot, struct.pack('<H', startY))
        data = self.readCommand(KBox.KommandScreenshotData)
        (y,) = struct.unpack('<H', data[0:2])
        pixelData = data[2:]
        pixels = [ KBox.convertToRgb(struct.unpack('<H', pixelData[i:i+2])[0]) for i in range(0, len(pixelData), 2) ]
        pixelsByLine = [ pixels[i:i+320] for i in range(0, len(pixels), 320) ]

        return (y, pixelsByLine)

    def takeScreenshot(self):
        t0 = time.time()
        capturedLines = 0
        pixels = []
        while capturedLines < 240:
            (y, rect) = self.captureScreen(capturedLines)
            capturedLines = capturedLines + len(rect)
            pixels.extend(rect)

        print "Captured screenshot with {} lines in {:.0f} ms".format(len(pixels), (time.time() - t0)*1000)
        png.from_array(pixels, 'RGB').save('screenshot.png')

    @staticmethod
    def convertToRgb(pixel):
        r = (pixel>>8)&0x00F8
        g = (pixel>>3)&0x00FC
        b = (pixel<<3)&0x00F8
        return (r,g,b)



def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", help = "USB Serial Port connected to KBox", default = "/dev/tty.usbmodem1461")
    parser.add_argument("--debug", action = "store_true", help = "Show all incoming packets")
    subparsers = parser.add_subparsers(dest = "command")
    subparsers.add_parser("ping")
    subparsers.add_parser("logs")
    subparsers.add_parser("screenshot")

    args = parser.parse_args()

    kbox = KBox(args.port, args.debug)

    # Wait until we can read one valid packet
    ready = False
    while not ready:
        try:
            p = kbox.read()
            ready = True
        except FatalError as e:
            print e

    if args.command == "ping":
        i = 0
        while True:
            kbox.ping(i)
            i = i + 1
            time.sleep(1)
    elif args.command == "logs":
        while True:
            # just keep waiting for a packet that does not exist
            # log messages will be printed automatically
            log = kbox.readCommand(KBox.KommandLog)
            kbox.printLog(log)
    elif args.command == "screenshot":
        capture = kbox.takeScreenshot()

if __name__ == '__main__':
    main()
