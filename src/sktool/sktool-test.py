#!/usr/bin/env python

import os
import sys
import subprocess
import json
import jsonschema
import argparse
from jsonschema.validators import validator_for, validate
from termcolor import colored

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
                    else:
                        raise ValueError("{} should have value {} but is {} instead.".format(v['path'], expected['value'], v['value']))
            if not found:
                raise ValueError("Could not find expected value with path {}".format(expected['path']))

        for v in values:
            if not 'tested' in v:
                raise ValueError("Found an extra value that was not expected with path {} and value {}".format(v['path'], v['value']))


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
        raise Exception("Not implemented. Use a subclass")

class SKTestRunnerSKTool(SKTestRunner):
    def name(self):
        return "SKTool"

    def fetchOutput(self, test):
        process = subprocess.Popen(['./.pioenvs/sktool/program'], stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        process.stdin.write(test.nmea)
        process.stdin.close()
        process.wait()
        (out, err) = (process.stdout.read(), process.stderr.read())
        return out

class SKTestRunnerSKOfficial(SKTestRunner):
    def name(self):
        return "SKOfficial"

    def fetchOutput(self, test):
        process = subprocess.Popen(['/Users/thomas/work/canboat/rel/darwin-x86_64/analyzer -json| /Users/thomas/work/n2k-signalk/bin/n2k-signalk --delta'], shell = True, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        process.stdin.write(test.nmea)
        process.stdin.close()
        process.wait()
        (out, err) = (process.stdout.read(), process.stderr.read())
        return out




def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--spec", help = "Where to find the SignalK spec delta.json file")
    parser.add_argument("--official", help = "Use SignalK official tools insted of KBox sktool", action = 'store_true')
    args = parser.parse_args()

    if args.official:
        runner = SKTestRunnerSKOfficial(args.spec)
    else:
        runner = SKTestRunnerSKTool(args.spec)

    workPath = os.path.dirname(sys.argv[0])

    testcases = []
    try:
        with open(os.path.join(workPath, 'tests/basic.json')) as f:
            testcases = json.load(f)
    except Exception as e:
        print("Unable to parse test cases: {}".format(e))

    testcases = map(lambda x: SKTestCase(x), testcases)

    for tc in testcases:
        runner.run(tc)

if __name__ == '__main__':
    main()
