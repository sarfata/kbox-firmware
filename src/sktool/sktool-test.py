#!/usr/bin/env python

import os
import sys
import subprocess
import json

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
    def run(self, test):
        process = subprocess.Popen(['./.pioenvs/sktool/program'], stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        process.stdin.write(test.nmea)
        process.stdin.close()
        process.wait()
        (out, err) = (process.stdout.read(), process.stderr.read())
        print("Sent: {}".format(test.nmea))
        print("Recv: {}".format(out.strip()))

        try:
            out = json.loads(out)
            # TODO: Run SignalK schema validator
            test.validateOutput(out)
            print "OK!\n"
        except ValueError as e:
            print("Error validating output: {}\n".format(e))


def main():
    runner = SKTestRunner()

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
