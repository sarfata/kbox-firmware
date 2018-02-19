#!/usr/bin/env python
# Copyright Thomas Sarlandie - 2017
# MIT License like the rest of KBox

import serial
import argparse
import struct
import time
import png
import random
import logging
import sys
import socket

""" Courtesy of esptool.py - GPL 

Modified to return a tuple (packer, Exception). Caller is responsible to decide
what to do with the Exception if it exits.
Kudos: https://stackoverflow.com/questions/11366892/handle-generator-exceptions-in-its-consumer

"""
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
            if partial_packet is None:
                logging.info("Timed out waiting for packet header")
                yield None
            else:
                logging.info("Timed out waiting for packet content")
                yield None

        for b in read_bytes:
            if partial_packet is None:  # waiting for packet header
                if b == '\xc0':
                    partial_packet = ""
                    connected = True
                else:
                    if connected:
                        raise KBoxError('Invalid head of packet (%r)' % b)
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
                    raise KBoxError('Invalid SLIP escape (%r%r)' % ('\xdb', b))
            elif b == '\xdb':  # start of escape sequence
                in_escape = True
            elif b == '\xc0':  # end of packet
                yield partial_packet
                partial_packet = None
            else:  # normal byte in packet
                partial_packet += b

class KBoxError(RuntimeError):
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
        return KBoxError(message + " [" +  ", ".join(hex(ord(x)) for x in
                                                    result) + "]")

class KBox(object):
    KommandPing = 0x00
    KommandPong = 0x01
    KommandErr = 0x0F
    KommandLog = 0x10
    KommandFileRead = 0x20
    KommandFileWrite = 0x21
    KommandFileReadReply = 0x22
    KommandFileError = 0x2F
    KommandScreenshot = 0x30
    KommandScreenshotData = 0x31
    KommandReboot = 0x33
    KommandWiFiStatus = 0x50
    KommandWiFiConfiguration = 0x51

    def __init__(self, port, debug = False):
        self._port = serial.Serial(port)
        self._port.baudrate = 1000000
        self._slip_reader = slip_reader(self._port)
        self._width = 320
        self._height = 240
        self._mtu = 15000;
        self._debug = debug

    def read(self):
        try:
            return self._slip_reader.next()
        except KBoxError as e:
            # reset reader on errors
            self._slip_reader = slip_reader(self._port)
            raise e

    def write(self, packet):
        """
        Write bytes to the serial port while performing SLIP escaping
        (Borrowed from esptool.py - thanks!)
        """
        buf = '\xc0' \
              + (packet.replace('\xdb','\xdb\xdd').replace('\xc0','\xdb\xdc')) \
              + '\xc0'
        self._port.write(buf)

    def readCommand(self, command, timeout = 1):
        """
        Reads incoming frame until a frame with specified command is received.
        """

        timer = time.time()

        while True:
            if time.time() > timer + timeout:
                raise KBoxError("timed out")

            try:
                frame = self.read()
            except KBoxError as e:
                logging.warn(e.message)
                continue

            if frame is None:
                continue

            if self._debug:
                print "> ({}) {}".format(len(frame), " ".join("{:02x}".format(ord(c)) for c in frame))

            if not frame or len(frame) < 2:
                raise KBoxError.WithFrame("Frame is too short to be valid", frame)

            (cmd,) = struct.unpack('<H', frame[0:2])

            if cmd == command:
                return frame[2:]
            elif cmd == KBox.KommandErr:
                raise KBoxError.WithFrame("KBox responded with an error", frame)
            elif cmd == KBox.KommandFileError:
                data = frame[2:]
                if len(data) != 8:
                    raise KBoxError.WithFrame("Invalid KommandFileError size {}"
                                              .format(len(data), frame))

                (fileid, errno) = struct.unpack('<LL', data)
                raise KBoxError.WithFrame("KBox responded with a file error {} "
                                           "for file {}".format(errno, fileid),
                                          frame)
            elif cmd == KBox.KommandLog:
                # Keep printing log messages while waiting
                self.printLog(frame[2:])
            elif cmd == KBox.KommandWiFiStatus:
                # Print content of WiFi Status messages
                self.printWiFiStatus(frame[2:])
            else:
                print("Got unexpected frame with id {}".format(cmd))

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

    def parseWiFiStatus(self, data):
        (state, dhcpClients, tcpClients, signalKClients) = struct.unpack('<HHHH', data[0:8])
        (ipAddress) = struct.unpack('!L', data[8:12])[0]

        print("ipaddress: {}".format(ipAddress))
        ipAddress = socket.inet_ntoa(struct.pack('!L', ipAddress))

        return state, dhcpClients, tcpClients, signalKClients, ipAddress

    def printWiFiStatus(self, data):
        (state, dhcpClients, tcpClients, signalKClients, ipAddress) = self.parseWiFiStatus(data)
        print("WIFI-STATUS: State: {} IP: {} Clients DHCP: {}, TCP: {}, SignalK: {}".format(state, ipAddress, dhcpClients,
                                                                                            tcpClients, signalKClients
                                                                                            ))

    def ping(self, pingId):
        print("PING[{}]".format(pingId))
        self.command(KBox.KommandPing, struct.pack('<I', pingId))
        t0 = time.time()

        ponged = False
        while not ponged:
            resp = self.readCommand(KBox.KommandPong)
            if len(resp) != 4:
                raise KBoxError("KBox ponged with wrong size frame ({} instead of 4)".format(len(resp)))
            else:
                (data,) = struct.unpack('<I', resp)
                print("PONG[{}] in {} ms".format(data, (time.time() - t0) * 1000))
                ponged = True

    def command(self, command, data = ""):
        logging.debug("Sending cmd {} len {}".format(command, len(data)))
        t0 = time.time()
        pkt = struct.pack('<H', command) + data
        self.write(pkt)

    def reboot(self):
        payload = "H0LDFA57" + '\0'
        self.command(KBox.KommandReboot, payload)

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
        pixels = [ KBox.convertToRgb(struct.unpack('<H', pixelData[i:i+2])[0])
                   for i in range(0, len(pixelData), 2) ]
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

        print "Captured screenshot with {} lines in {:.0f} ms"\
                .format(len(pixels), (time.time() - t0)*1000)
        return png.from_array(pixels, 'RGB')

    def read_file(self, filename):
        t0 = time.time()

        data = ""
        block_count = 0
        while True:
            block = self.read_file_block(filename, len(data))
            block_count = block_count + 1
            data = data + block
            if len(block) == 0:
                break

        duration = time.time() - t0
        speed = len(data) / duration
        logging.info("Read file {} ({} bytes) in {}ms. {} kB/s. ({} blocks)"
                     .format(filename, len(data), duration * 1000, speed/1024,
                             block_count))
        return data

    def read_file_block(self, filename, start_position = 0, size = 32 * 1024 *
                                                               1024):
        read_id = int(random.random() * 2**32)

        request = struct.pack('<LLL', read_id, start_position, size)
        request = request + filename + '\0'

        self.command(KBox.KommandFileRead, request)

        data = self.readCommand(KBox.KommandFileReadReply)

        (reply_read_id, reply_size) = struct.unpack('<LL', data[0:8])

        if reply_read_id != read_id:
            raise KBoxError.WithFrame("Got a read reply for another read ({"
                                       "} <> {})".format(hex(reply_read_id),
                                                         hex(read_id)),
                                      data)

        if len(data) != reply_size + 8:
            raise KBoxError.WithFrame("Invalid reply length - Got {} bytes. "
                                        "Should be {}+8={}"
                                      .format(len(data), reply_size,
                                                reply_size+8),
                                      data)

        logging.debug("Read {} bytes from pos {}".format(reply_size,
                                                     start_position))
        return data[8:8+reply_size]

    def write_file(self, filename, data, block_size = 2000):
        t0 = time.time()
        bytes_sent = 0
        block_count = 0
        while bytes_sent < len(data):
            remaining_bytes = len(data) - bytes_sent
            if remaining_bytes > block_size:
                remaining_bytes = block_size

            block = data[bytes_sent:bytes_sent + remaining_bytes]
            self.write_file_block(filename, block, bytes_sent)
            bytes_sent = bytes_sent + remaining_bytes
            block_count = block_count + 1

        duration = time.time() - t0
        speed = len(data) / duration

        logging.info("Wrote file {} ({} bytes) in {}ms. {} kB/s. ({} blocks)"
                     .format(filename, len(data), duration * 1000, speed / 1024,
                             block_count))

    def write_file_block(self, filename, data, start_position = 0, retries = 3):
        write_id = int(random.random() * 2**32)

        request = struct.pack('<LLL', write_id, start_position, len(data))
        request = request + filename + '\0'
        request = request + data

        while retries > 0:
            self.command(KBox.KommandFileWrite, request)

            try:
                data = self.readCommand(KBox.KommandFileError, timeout = 0.1)
            except KBoxError as e:
                logging.warn(e.message)
                retries = retries - 1
                continue

            (reply_write_id, reply_error) = struct.unpack('<LL', data[0:8])
            if reply_write_id != write_id:
                raise KBoxError.WithFrame("Got a file error for another operation"
                                          "({})".format(reply_read_id), data)
            if reply_error != 0:
                raise KBoxError.WithFrame("Write Error ({})".format(reply_error),
                                          data)
            # success!
            return

        raise KBoxError("Failed to write block - retries exceeded")

    def send_wifi_config(self, apSSID, apPassword, clientSSID, clientPassword, vesselURN):
        if apSSID is not None:
            request = struct.pack('<?', True)
            request = request + apSSID + '\0' + apPassword + '\0'
        else:
            request = struct.pack('<?, False')
            request = request + '\0\0'

        if clientSSID is not None:
            request = request + struct.pack('<?', True)
            request = request + clientSSID + '\0' + clientPassword + '\0'
        else:
            request = request + struct.pack('<?', False)
            request = request + '\0\0'

        request = request + vesselURN + '\0'

        self.command(KBox.KommandWiFiConfiguration, request)

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
    subparsers.add_parser("reboot")

    screenshot_parser = subparsers.add_parser("screenshot")
    screenshot_parser.add_argument("filename", default = 'screenshot.png')

    file_read_parser = subparsers.add_parser("fread")
    file_read_parser.add_argument("filename")
    file_read_parser.add_argument("destination", type = argparse.FileType('w'),
                                  default = sys.stdout, nargs = '?')

    file_write_parser = subparsers.add_parser("fwrite")
    file_write_parser.add_argument("filename")
    file_write_parser.add_argument("destination", nargs = '?')

    wifi_parser = subparsers.add_parser("wificonfig")
    wifi_parser.add_argument("--ap-ssid")
    wifi_parser.add_argument("--ap-password")
    wifi_parser.add_argument("--client-ssid")
    wifi_parser.add_argument("--client-password")
    wifi_parser.add_argument("vesselurn")

    args = parser.parse_args()

    if args.debug:
        logging.basicConfig(level = logging.DEBUG)
    else:
        logging.basicConfig(level = logging.INFO)

    kbox = KBox(args.port, args.debug)

    # Wait until we can read one valid packet
    ready = False
    while not ready:
        try:
            p = kbox.read()
            ready = True
        except KBoxError as e:
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
    elif args.command == "reboot":
        kbox.reboot()
    elif args.command == "screenshot":
        image = kbox.takeScreenshot()
        image.save(args.filename)

    elif args.command == "fread":
        data = kbox.read_file(args.filename)
        args.destination.write(data)

    elif args.command == "fwrite":
        with open(args.filename, 'r') as f:
            data = f.read()
            destination = args.filename
            if args.destination:
                destination = args.destination
            kbox.write_file(destination, data)

    elif args.command == "wificonfig":
        kbox.send_wifi_config(args.ap_ssid, args.ap_password, args.client_ssid, args.client_password, args.vesselurn)

        while True:
            status = kbox.readCommand(KBox.KommandWiFiStatus)
            kbox.printWiFiStatus(status)


if __name__ == '__main__':
    main()
