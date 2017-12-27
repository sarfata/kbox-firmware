#!/usr/bin/env python
import argparse
import time
import re
import serial
from termcolor import colored
from kbox import KBox,KBoxError

"""
Helper methods to interact with KBox.
"""
class KBoxJig(object):
    def __init__(self, nmea1serial):
        self.nmea1serial = serial.Serial(nmea1serial)
        self.nmea1serial.baudrate = 38400

    """
    Send a NMEA Sentence to the NMEA1 input of KBox.
    """
    def sendNMEA1Sentence(self, sentence):
        print(colored("NMEA1>", 'white', 'on_yellow', attrs = ['bold']) + " " + colored(sentence, 'white'))
        self.nmea1serial.write(sentence + "\r\n")

"""
Base class for a KBox test.
"""
class KBoxTest(object):
    def __init__(self, kbox, kboxjig):
        self.kbox = kbox
        self.kboxjig = kboxjig
        self.debug = False

    """
    Search logs until the specific regexp matches.

    Returns the match object or None if the regexp is not found before the timeout
    expires.
    """
    def searchLog(self, logpattern, levelpattern = ".*", fnamepattern = ".*", timeout = 3):
        timerExpiry = time.time() + timeout

        logre = re.compile(logpattern)
        fnamere = re.compile(fnamepattern)
        levelre = re.compile(levelpattern)

        while (time.time() < timerExpiry):
            log = self.kbox.readCommand(KBox.KommandLog)
            (loglevel, fname, lineno, log) = self.kbox.parseLogCommand(log)

            formattedLog = "> {} {}:{} {}".format(loglevel, fname, lineno, log)

            if fnamere.search(fname) and levelre.search(loglevel):
                m = logre.search(log)
                if m:
                    print(colored(formattedLog, 'green'))
                    return m
            print(colored(formattedLog, 'magenta'))

        # timer expired
        return None

    """
    Run the test. Return true if the test is successful or false otherwise.
    """
    def run(self):
        return False

class BasicRMCTest(KBoxTest):
    def run(self):
        # Look for a TaskManager message to make sure we have started
        if not self.searchLog(".*", fnamepattern = "TaskManager.cpp", timeout = 10):
            return False

        self.kboxjig.sendNMEA1Sentence("$GPRMC,004119.000,A,3751.3385,N,12227.4913,W,5.02,235.24,141116,,,D*75")

        if not self.searchLog("Found .* sentences waiting", fnamepattern = "NMEAService"):
            return False
        if not self.searchLog("Sending message on n2k bus - pgn=129026", fnamepattern = "NMEA2000Service.cpp"):
            return False
        if not self.searchLog("TX: \\$PCDIN,01F802", fnamepattern = "NMEA2000Service.cpp"):
            return False

        # Look for a TaskManager message to make sure we have not crashed.
        if not self.searchLog(".*", fnamepattern = "TaskManager.cpp", timeout = 10):
            return False

        return True

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--nmea1-serial", default = "/dev/tty.usbserial")
    parser.add_argument("--usb-serial", default = "/dev/tty.usbmodem1461")
    parser.add_argument("--ip")

    args = parser.parse_args()

    kboxjig = KBoxJig(args.nmea1_serial)

    ready = False
    while (not ready):
        try:
            kbox = KBox(args.usb_serial)
            p = kbox.ping(42)
            ready = True
            print "Connected!"
        except KBoxError as e:
            print e

    tests = []
    tests.append(BasicRMCTest(kbox, kboxjig))

    for t in tests:
        print("Running test {}".format(t.__class__.__name__))
        if t.run():
            print(colored("PASS", 'green'))
        else:
            print(colored("FAIL", 'red'))


if __name__ == '__main__':
    main()
