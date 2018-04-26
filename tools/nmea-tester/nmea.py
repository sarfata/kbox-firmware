#!/usr/bin/env python

# This file is part of KBox.
#
# Copyright (C) 2016 Thomas Sarlandie.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


import serial
import time
import argparse
import re
import sys

# Returns pair of ts (float, in seconds) and NMEA sentences
def nmea_read(input):
    p = re.compile("^([0-9.]+):(.*)")
    while True:
        line = input.readline()
        if not line:
            break
        line = line.strip(" \r\n")
        if line != "":
            ts = 0
            sentence = line

            m = p.match(line)
            if m:
                ts = float(m.group(1))
                sentence = m.group(2)
            elif "," in line:
                (ts,sentence) = line.split(',', 1)
                ts = float(ts) / 1000

            yield(ts, sentence)

            # (ts, sentence) = line.split(':', 1)
            # ts = float(ts) #/ 1000
            # if ts is not None and sentence is not None:
                # yield (ts, sentence)

def nmea_read_with_delay(input):
    start = time.time()
    # Timestamps in file are often arbitrary (and in past) so we need to adjust them
    delta = 0
    for (ts, sentence) in nmea_read(input):
        if delta == 0:
            delta = ts

        # block until time is reached
        delay = ts - delta - (time.time() - start)

        if delay > 0:
            time.sleep(delay)
        yield sentence

def send_sentence(output, nmea):
    print nmea
    output.write(nmea + "\r\n")

def replay(input, output):
    for nmea in nmea_read_with_delay(input):
        output.reset_input_buffer()
        send_sentence(output, nmea)

def nmea_checksum(data):
    checksum = 0

    for c in data:
        checksum = checksum ^ ord(c)

    return checksum

def validate(input):
    sentences = 0
    invalid = 0
    types = {}

    pattern = re.compile('^(.)(.*)\*([0-9A-F]*)$')
    for (ts, sentence) in nmea_read(input):
        sentences = sentences + 1
        m = pattern.match(sentence)
        if m:
            prefix = m.group(1)
            data = m.group(2)
            checksum = m.group(3)

            csum = nmea_checksum(data)

            if checksum != '':
                original_csum = int(checksum, 16)

                if original_csum != csum:
                    invalid = invalid + 1
                    print "Invalid checksum (calculated: %02X): %s" % (csum, sentence)

        else:
            invalid = invalid + 1
            print "Invalid: {}".format(sentence)

    print "Found {} sentences with {} invalid lines.".format(sentences, invalid)

def record(input, output):
    while True:
        line = input.readline()
        if not line:
            break
        line = line.strip(' \r\n')
        output.write(str(time.time()) + ":" + line + "\n")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('command', choices=[ 'validate', 'replay', 'record' ])
    parser.add_argument('--port', help = 'Serial port', default = None)
    parser.add_argument('--baud', help = 'Baud rate', default = 38400)
    parser.add_argument('--input', type=argparse.FileType('r'), default=sys.stdin,
            help = 'Input file to read. Format is <timestamp in ms>:<NMEA Line>')
    parser.add_argument('--output', type=argparse.FileType('w'), default=sys.stdout,
            help = 'Output file to write to. Format is <timestamp in ms>:<NMEA Line>')

    args = parser.parse_args()

    port = None
    if args.port:
        port = serial.Serial(port = args.port, baudrate = args.baud, timeout = 1, write_timeout = 1, xonxoff=False, rtscts=False, dsrdtr=False)

    if args.command == 'replay':
        if port:
            replay(args.input, port)
        else:
            replay(args.input, args.output)
    elif args.command == 'record':
        if port:
            record(port, args.output)
        else:
            # useful with nc <server> |nmea.py record --input=-
            record(args.input, args.output)
    elif args.command == 'validate':
        if port:
            validate(port)
        else:
            validate(args.input)
    else:
        print "Invalid command! {}".format(args.command)


if __name__ == '__main__':
    main()
