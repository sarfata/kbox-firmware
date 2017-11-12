#!/usr/bin/env python

import os
import sys
import subprocess
import json
import jsonschema
import argparse
from jsonschema.validators import validator_for, validate
from termcolor import colored

# Courtesy of https://stackoverflow.com/questions/5595425
def isclose(a, b, rel_tol=1e-09, abs_tol=0.0):
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)

class SKTestCase(object):
    def __init__(self, json):
        self.nmea = json['nmea']
        self.expectedValues = json['expectedValues']

    def validateOutput(self, json):
        if not 'updates' in json:
            raise ValueError("Missing 'updates' key")
        if len(json['updates']) != 1:
            raise ValueError("Expected 1 update but found {} updates".format(len(json['updates'])))
        if not 'values' in json['updates'][0]:
            raise ValueError("No 'values' found")

        values = json['updates'][0]['values']

        for expected in self.expectedValues:
            found = False
            for v in values:
                if v['path'] == expected['path']:
                    found = True
                    v['tested'] = True

                    if v['value'] == expected['value']:
                        pass
                    elif isinstance(expected['value'], float) and isclose(v['value'], expected['value']):
                        pass
                    else:
                        raise ValueError("{} should have value {} but is {} instead.".format(v['path'], expected['value'], v['value']))
            if not found:
                raise ValueError("Could not find expected value with path {}".format(expected['path']))

        for v in values:
            if not 'tested' in v:
                raise ValueError("Found an extra value that was not expected with path {} and value {}".format(v['path'], v['value']))

class SKRunnerError(Exception):
    pass

class SKTestRunner(object):
    def __init__(self, skspecPath):
        self.schema = json.load(open(skspecPath))

    def run(self, test):
        out = self.fetchOutput(test)
        print(colored("Testing:", 'white', 'on_cyan') + " " + colored(test.nmea, 'cyan'))

        try:
            out = json.loads(out)
            print(colored(json.dumps(out, sort_keys=True, indent=4, separators=(',', ': ')), 'blue'))
            try:
                validate(out, self.schema)
            except jsonschema.exceptions.RefResolutionError as e:
                print(colored("Schema reference resolution error (are you connected?) - continuing...", 'yellow'))
            test.validateOutput(out)
            print(colored("PASS", 'white', 'on_green'))
        except ValueError as e:
            print(colored("FAIL", 'white', 'on_red') + " " + colored("Error parsing JSON: {}.".format(e), 'red'))
            print(colored(out, 'magenta'))
        except jsonschema.exceptions.ValidationError as e:
            print(colored("FAIL", 'white', 'on_red') + " " + colored("Error: JSON does not validate signalk specification", 'red'))
            print(colored(e.message, 'red'))
            print(colored(out, 'magenta'))

        print ""

    def fetchOutput(self, test):
        raise SKRunnerError("Not implemented. Use a subclass")

class SKTestRunnerSKTool(SKTestRunner):
    def name(self):
        return "SKTool"

    def fetchOutput(self, test):
        try:
            process = subprocess.Popen(['./.pioenvs/sktool/program'], stdin = subprocess.PIPE,
                    stdout = subprocess.PIPE, stderr = subprocess.PIPE)
            process.stdin.write(test.nmea)
            process.stdin.close()
            process.wait()
            (out, err) = (process.stdout.read(), process.stderr.read())
            return out
        except OSError as error:
            raise SKRunnerError("Unable to run sktool. Please run this script from the kbox"
                    "root folder (OSError {}: {})".format(error.errno,
                        error.strerror))

class SKTestRunnerSKOfficial(SKTestRunner):
    def __init__(self, skspecPath, canboat = None, n2k_signalk = None,
            nmea0183_signalk = None):
        SKTestRunner.__init__(self, skspecPath)
        if canboat:
            self.canboat = canboat
        else:
            self.canboat = 'canboat'
        if n2k_signalk:
            self.n2k_signalk = n2k_signalk
        else:
            self.n2k_signalk = 'n2k-signalk'
        if nmea0183_signalk:
            self.nmea0183_signalk = nmea0183_signalk
        else:
            self.nmea0183_signalk = 'nmea0183-signalk'

    def name(self):
        return "SKOfficial"

    def fetchOutput(self, test):
        process = None

        if "$PCDIN" in test.nmea:
            process = subprocess.Popen(['{} -json| {} --delta'.format(self.canboat, self.n2k_signalk)],
                shell = True, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        else:
            try:
                process = subprocess.Popen([ self.nmea0183_signalk ], stdin = subprocess.PIPE,
                        stdout = subprocess.PIPE, stderr = subprocess.PIPE)
            except OSError as error:
                raise SKRunnerError("Unable to run signalk-parser-nmea0183. If it's not in your path, please indicate "
                                    "where it is with the --nmea0183-signalk argument. (OSError {}: {})".format(error.errno,
                                        error.strerror))
        process.stdin.write(test.nmea)
        process.stdin.close()
        process.wait()
        (out, err) = (process.stdout.read(), process.stderr.read())
        return out




def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--spec", help = "Where to find the SignalK spec delta.json file", required = True)
    parser.add_argument("--implementation", choices=['sktool', 'signalk'], required = True)
    parser.add_argument("--canboat", help = "Where to find canboat/analyzer")
    parser.add_argument("--n2k-signalk", help = "Where to find n2k-signalk"
            "project")
    parser.add_argument("--nmea0183-signalk", help = "Where to find"
        "nmea0183-signalk (from signalk/signalk-parser-nmea0183)")
    args = parser.parse_args()

    if args.implementation == 'signalk':
        runner = SKTestRunnerSKOfficial(args.spec, canboat = args.canboat,
                n2k_signalk = args.n2k_signalk, nmea0183_signalk = args.nmea0183_signalk)
    elif args.implementation == 'sktool':
        runner = SKTestRunnerSKTool(args.spec)
    else:
        print(colored("Unknown test implementation: {}".format(args.implementation), 'red'))
        return

    workPath = os.path.dirname(sys.argv[0])

    testcases = []
    try:
        with open(os.path.join(workPath, 'tests/basic.json')) as f:
            testcases = json.load(f)
    except Exception as e:
        print("Unable to parse test cases: {}".format(e))

    testcases = map(lambda x: SKTestCase(x), testcases)

    try:
        for tc in testcases:
            runner.run(tc)
    except SKRunnerError as e:
        print(colored(e.message, 'red'))

if __name__ == '__main__':
    main()
