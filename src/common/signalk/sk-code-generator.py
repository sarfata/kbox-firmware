#!/usr/bin/env python

import argparse

class SKModel(object):
    def __init__(self, keys):
        self.keys = keys

class SKKey(object):
    def __init__(self, path, skType, description):
        self.path = path
        self.skType = skType
        self.description = description

    def getPath(self):
        return self.path

    def getSKType(self):
        return self.skType

    def getDescription(self):
        return self.description



class SKPathGenerator():
    def generate():
        pass

def main():
    parser = argparse.ArgumentParser()
    args = parser.parse_args()

if __name__ == '__main__':
    main()
