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
    def WithResult(message, result):
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

    def __init__(self, port):
        self._port = serial.Serial(port)
        self._port.baudrate = 1000000
        self._slip_reader = slip_reader(self._port)
        self._width = 320
        self._height = 240
        self._mtu = 15000;

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
            if len(frame) < 2:
                raise FatalError("Frame is too short to be valid", frame)

            (cmd,) = struct.unpack('<H', frame[0:2])

            if cmd == command:
                return frame[2:]
            elif cmd == KBox.KommandLog:
                self.processLogFrame(frame[2:])
            else:
                print("Got unexpected command with id {}".format(cmd))

    def processLogFrame(self, data):
        (level, lineno, fnamelen) = struct.unpack('<HHH', data[0:6])
        fname = data[6:6+fnamelen]
        log = data[6+fnamelen:]

        print("> {} {}:{} {}".format(level, fname, lineno, log))


    def ping(self, pingId):
        self.command(KBox.KommandPing, struct.pack('<I', pingId))
        t0 = time.time()

        ponged = False
        while not ponged:
            resp = self.readCommand(KBox.KommandPong)
            (data) = struct.unpack('<I', resp)
            print("PONG {} in {} ms".format(data, (time.time() - t0) * 1000))
            ponged = True

    def command(self, command, data = ""):
        print("Sending cmd {} len {}".format(command, len(data)))
        t0 = time.time()
        pkt = struct.pack('<H', command) + data
        self.write(pkt)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", help = "USB Serial Port connected to KBox", default = "/dev/tty.usbmodem1441")
    subparsers = parser.add_subparsers(dest = "command")
    subparsers.add_parser("ping")
    args = parser.parse_args()

    kbox = KBox(args.port)

    # Wait until we can read one valid packet
    ready = False
    while not ready:
        try:
            p = kbox.read()
            ready = True
        except FatalError as e:
            print e
        except StopIteration as e:
            print e


    if args.command == "ping":
        i = 0
        while True:
            kbox.ping(i)
            i = i + 1
            time.sleep(1)



if __name__ == '__main__':
    main()
