#!/usr/bin/env python

from __future__ import print_function
import os
import sys
import subprocess
import json
import jsonschema
import argparse
import urllib2
from jsonschema.validators import validator_for, validate
from termcolor import colored

# Courtesy of https://stackoverflow.com/questions/5595425
def isclose(a, b, rel_tol=1e-09, abs_tol=0.0):
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)

class SKTestCase(object):
    def __init__(self, json):
        self.nmea = json['nmea']
        self.expectedValues = json['expectedValues']
        if 'ignoreFail' in json and json['ignoreFail']:
            self.ignoreFail = True
        else:
            self.ignoreFail = False

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
                    elif isinstance(expected['value'], float) and isclose(v['value'], expected['value'], abs_tol = 0.00001):
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
    def __init__(self, skspecPath, verbose = False):
        self.schema = json.load(urllib2.urlopen(skspecPath))
        self.verbose = verbose

    def run(self, test):
        out = self.fetchOutput(test)
        print(colored("Testing:", 'white', 'on_cyan') + " " + colored(test.nmea, 'cyan'), end='')

        try:
            out = json.loads(out)
            if self.verbose:
                print(colored(json.dumps(out, sort_keys=True, indent=4, separators=(',', ': ')), 'blue'))
            try:
                validate(out, self.schema)
            except jsonschema.exceptions.RefResolutionError as e:
                if self.verbose:
                    print(colored("Schema reference resolution error (are you connected?) - continuing...", 'yellow'))
            test.validateOutput(out)
            print(" " + colored("PASS", 'white', 'on_green'))
            return True
        except ValueError as e:
            print(" " + colored("FAIL", 'white', 'on_red'))
            print(colored(out, 'magenta'))
            print(colored("Error parsing JSON: {}.".format(e), 'red'))
            return False
        except jsonschema.exceptions.ValidationError as e:
            print(" " + colored("FAIL", 'white', 'on_red'))
            print(colored(out, 'magenta'))
            print(colored("Error: JSON does not validate signalk specification", 'red'))
            print(colored(e.message, 'red'))
            return False

    def runCases(self, testcases):
        passCount = 0
        failCount = 0
        ignoreFailCount = 0
        errorCount = 0

        for tc in testcases:
            try:
                if self.run(tc):
                    passCount = passCount + 1
                else:
                    if tc.ignoreFail:
                        ignoreFailCount = ignoreFailCount + 1
                    else:
                        failCount = failCount + 1
            except SKRunnerError as e:
                errorCount = errorCount + 1
                print(colored(e.message, 'red'))

        return (passCount, failCount, ignoreFailCount, errorCount)

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
            nmea0183_signalk = None, verbose = False):
        SKTestRunner.__init__(self, skspecPath, verbose)
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
            canboat_process = None
            try:
                canboat_process = subprocess.Popen([ self.canboat, '-json' ], stdin = subprocess.PIPE,
                        stdout = subprocess.PIPE, stderr = subprocess.PIPE)
            except OSError as error:
                raise SKRunnerError("Unable to run canboat/analyzer - Use --canboat to point to it. (OSError {}: {})".format(
                    error.errno, error.strerror))
            try:
                n2k_signalk_process = subprocess.Popen([ self.n2k_signalk ], stdin = canboat_process.stdout,
                        stdout = subprocess.PIPE, stderr = subprocess.PIPE)
            except OSError as error:
                raise SKRunnerError("Unable to run n2k-signalk - Use --n2k-signalk to point to it. (OSError {}: {})".format(
                    error.errno, error.strerror))

            canboat_process.stdin.write(test.nmea)
            canboat_process.stdin.close()
            canboat_process.wait()
            n2k_signalk_process.wait()
            (out, err) = (n2k_signalk_process.stdout.read(), n2k_signalk_process.stderr.read())
            return out
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
    parser.add_argument("--spec", help = "Where to find the SignalK spec delta.json file",
            default = "https://raw.githubusercontent.com/SignalK/specification/master/schemas/delta.json")
    parser.add_argument("--implementation", choices=['sktool', 'signalk'], default = 'sktool')
    parser.add_argument("--canboat", help = "Where to find canboat/analyzer")
    parser.add_argument("--n2k-signalk", help = "Where to find n2k-signalk"
            "project")
    parser.add_argument("--nmea0183-signalk", help = "Where to find"
        "nmea0183-signalk (from signalk/signalk-parser-nmea0183)")
    parser.add_argument("-v", "--verbose", help = "More verbose output", action = 'store_true')
    parser.add_argument("tests", help = "A JSON file with NMEA input and expected output in SignalK",
            type = argparse.FileType('r'), nargs = '+')

    args = parser.parse_args()

    if args.implementation == 'signalk':
        runner = SKTestRunnerSKOfficial(args.spec, canboat = args.canboat,
                n2k_signalk = args.n2k_signalk, nmea0183_signalk = args.nmea0183_signalk, verbose = args.verbose)
    elif args.implementation == 'sktool':
        runner = SKTestRunnerSKTool(args.spec, verbose = args.verbose)
    else:
        print(colored("Unknown test implementation: {}".format(args.implementation), 'red'))
        return

    workPath = os.path.dirname(sys.argv[0])

    testsuites = []
    for f in args.tests:
        try:
            cases = json.load(f)
            suite = { "name": f.name, "testcases": map(lambda x: SKTestCase(x), cases) }
            testsuites.append(suite)
        except Exception as e:
            print("Unable to parse test cases: {}".format(e))

    (totalPass, totalFail, totalIgnore, totalError) = (0, 0, 0, 0)
    for suite in testsuites:
        print(colored("Running tests in {}".format(suite['name']), 'white', 'on_blue'))
        (passCount, failCount, ignoreFailCount, errorCount) = runner.runCases(suite['testcases'])
        total = passCount + failCount + ignoreFailCount + errorCount

        print(colored("{} pass out of {} tests. {} tests failed ({} ignored) - {} tests error".format(passCount, total,
            failCount + ignoreFailCount, ignoreFailCount, errorCount), "white", "on_blue"))
        print()

        totalPass = totalPass + passCount
        totalFail = totalFail + failCount
        totalIgnore = totalIgnore + ignoreFailCount
        totalError = totalError + errorCount

    total = totalFail + totalPass + totalError + totalIgnore
    print("{} PASS out of {} tests. {} tests failed ({} ignored) - {} tests error".format(totalPass, total,
        totalFail + totalIgnore, totalFail, totalError))

    if totalFail + totalError > 0:
        sys.exit(-1)

if __name__ == '__main__':
    main()
