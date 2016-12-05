#!/usr/bin/env python

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
    CMD_SYNC = 0
    CMD_RESET = 1
    CMD_DRAW_PIXELS = 2
    CMD_ACK = 3

    def __init__(self, port):
        self._port = serial.Serial(port)
        self._port.baudrate = 115200
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

    def command(self, command, data = ""):
        print("Sending cmd {} len {}".format(command, len(data)))
        t0 = time.time()
        pkt = struct.pack('<HH', command, len(data)) + data
        self.write(pkt)
        t1 = time.time()

        acked = False

        while not acked:
            try:
                resp = self.read()
                if len(resp) >= 4:
                    (cmd, length) = struct.unpack('<HH', resp[:4])
                    if cmd == RemoteDisplay.CMD_ACK and length == 12:
                        (ackedCommand, errorCode, elapsedUs) = struct.unpack('<HHQ', resp[4:])
                        if ackedCommand == command:
                            acked = True
                            t2 = time.time()
                            print("Acked. Return {} in {}us - TX={:.3f}ms RX={:.3f}ms Total={:.3f}ms".format(errorCode, elapsedUs, (t1-t0)*1000, (t2-t1)*1000, (t2-t0)*1000))
                        else:
                            print("Received an ack for another command {} - Return {} in {}us".format(ackedCommand, errorCode, elapsedUs))
                    else:
                        print("Received unexpected reply cmd={} len={}".format(cmd, length))
                else:
                    print("Received frame is too small! len={}".format(len(resp)))
            except StopIteration:
                pass
            except FatalError as e:
                print "Error :/" + e.message
                pass

    def sync(self):
        self.command(RemoteDisplay.CMD_SYNC)

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

    def png(self, pngFile):
        """
        Opens a PNG and draws it on the screen. 
        """

        (w, h, pixels, metadata) = png.Reader(pngFile).read()
 
        print "PNG w={} h={} metadata={}".format(w, h, metadata)

        if w > self._width:
            w = self._width
        if h > self._height:
            h = self._height

        x = (320 - w) / 2
        y = (240 - h) / 2


        # Calculate how many lines to send per slip frame
        overhead = 8
        bytes_per_line = self._width * 2
        lines_per_frame = (self._mtu - overhead) / bytes_per_line

        print("Will attempt {} lines per frame ({} bytes)".format(lines_per_frame, overhead+bytes_per_line * lines_per_frame))

        # Prepare blocks of data in advance (because this actually takes some time)
        chunks = []


        data = []
        lines_in_frame = 0
        t0 = tframe = time.time()
        for line in pixels:
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
            lines_in_frame = lines_in_frame + 1
            if lines_in_frame >= lines_per_frame:
                pixels = [ struct.pack('<H', RemoteDisplay.color565(p)) for p in data ]
                chunks.append((y, lines_in_frame, pixels))
                y = y + lines_in_frame
                lines_in_frame = 0
                data = []
                print("Block prepared in {}ms".format((time.time() - tframe)*1000))
                tframe = time.time()

        # push whatever is left in pipe
        if len(data) > 0:
            pixels = [ struct.pack('<H', RemoteDisplay.color565(p)) for p in data ]
            chunks.append((y, lines_in_frame, pixels))
            print("Block prepared in {}ms".format((time.time() - tframe)*1000))

        tx = time.time()
        for chunk in chunks:
            (dy, h, data) = chunk
            self.command(RemoteDisplay.CMD_DRAW_PIXELS, struct.pack('<HHHH', x, dy, w, h) + "".join(data))

        print("Image prepared in {:.2f}ms - Sent in {:.2f}ms - Total {:.2f}ms".format((tx - t0)*1000, (time.time() - tx)*1000, (time.time() - t0)*1000))

    @staticmethod
    def color565(rgb):
        r = (rgb & 0xff0000) >> 16;
        g = (rgb & 0xff00) >> 8;
        b = rgb & 0xff;

        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

def png_wrapper(display, png):
    def wrapped():
        display.png(png)
    return wrapped

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("--port", help = "USB Serial Port connected to KBox", default = "/dev/tty.usbmodem1461")
    parser.add_argument("--time", action="store_true", help = "Measure time taken")

    parser.add_argument("png")

    args = parser.parse_args()

    d = RemoteDisplay(args.port)

    d.sync()

    d.reset()
    if (args.time):
        t = timeit.timeit(png_wrapper(d, args.png), number = 1)
        print("PNG Drawing took {}s".format(t))
    else:
        d.png(args.png)

if __name__ == '__main__':
    main()
