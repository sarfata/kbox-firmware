#!/usr/bin/python

import argparse
import re
import sys

class DataPoint:
    def __init__(self, ts, data):
        self.ts = ts
        self.data = data

def next_line(file):
    while True:
        line = file.readline()
        if not line:
            break

        line = line.strip("\r\n")

        if line != "":
            (ts, data) = line.split(",", 1)

            yield(DataPoint(int(ts), data))

class VoltageParser:
    def __init__(self):
        self.pattern = re.compile("^Voltage '(.*)': ([0-9.]*)$")

    def parse(self, data):
        m = self.pattern.match(data)
        if m:
            voltage = float(m.group(2))
            label = m.group(1)
            return { 'voltage-' + label: voltage }
        return None

class IMUParser:
    def __init__(self):
        self.pattern = re.compile("^Calibration: (.*) Course: ([0-9]+) MAG Pitch: ([0-9.-]+) Heel: ([0-9.-]+)$")

    def parse(self, data):
        m = self.pattern.match(data)
        if m:
            return { 'imu-calibration': m.group(1), 'imu-course': m.group(2), 'imu-pitch': m.group(3), 'imu-heel': m.group(4)}
        return None


class PressureParser:
    def __init__(self):
        self.pattern = re.compile("^Current pressure is: ([0-9.]*)$")

    def parse(self, data):
        m = self.pattern.match(data)
        if m:
            return { 'pressure': float(m.group(1)) }
        return None

class RMCParser:
    def __init__(self):
        pass

    def parse(self, data):
        if data.startswith("$") and data[3:6] == "RMC":
            fields = data.split(",")
            if len(fields) != 13:
                # discard if the sentence is incomplete. 
                return None
            return { 'gps-time': fields[1], 'gps-validity': fields[2], 'gps-latitude': fields[3] + fields[4],
                    'gps-longitude': fields[5] + fields[6], 'gps-SOG': fields[7], 'gps-COG': fields[8],
                    'gps-date': fields[9], 'gps-variation': fields[10] }

class LogFormatter:
    def __init__(self, file, delay, columns):
        self.out = file
        self.update_delay = delay
        self.columns = columns

        self.interval = 0
        self.first_ts = None
        self.parsers = [ VoltageParser(), PressureParser(), RMCParser(), IMUParser() ]
        self.current = {}
        self.detectedColumns = {}

        if self.out:
            self.out.write("interval")
            for col in self.columns:
                self.out.write(",")
                self.out.write(col)
            self.out.write("\n")

    def add_data(self, ts, data):
        if self.first_ts is None:
            self.first_ts = ts
        if ts > self.first_ts + (self.interval+1) * self.update_delay:
            self.generate_update()

        for p in self.parsers:
            keyValues = p.parse(data)
            if keyValues is not None:
                self.current.update(keyValues)

                for c in keyValues.keys():
                    self.detectedColumns[c] = True

    def generate_update(self):
        if self.out is None:
            return

        self.out.write(str(self.interval))
        for col in self.columns:
            self.out.write(",")
            if col in self.current:
                self.out.write(str(self.current[col]))
        self.out.write("\n")
        self.current = {}
        self.interval = self.interval + 1

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', type=argparse.FileType('r'), default=sys.stdin)
    parser.add_argument('--output', type=argparse.FileType('w'), default=sys.stdout)
    parser.add_argument('--auto-header', type=int, default=10000)
    parser.add_argument('--interval', type=int, default = 60, help = "Interval in seconds between lines")
    args = parser.parse_args()

    # First auto-detect header by reading the data
    logger = LogFormatter(None, 10000, [])

    i = 0
    for dataPoint in next_line(args.input):
        logger.add_data(dataPoint.ts, dataPoint.data)
        i = i+1
        if i > args.auto_header:
            break

    detectedColumns = logger.detectedColumns.keys()
    logger = LogFormatter(args.output, args.interval * 1000, sorted(detectedColumns))
    for dataPoint in next_line(args.input):
        logger.add_data(dataPoint.ts, dataPoint.data)

if __name__ == '__main__':
    main()

