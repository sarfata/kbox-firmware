from SCons.Script import DefaultEnvironment
import os

env = DefaultEnvironment()

print "COMPILER: " + env['CXX']

env.Replace(
    CC=os.environ.get('CC', env['CC']),
    CXX=os.environ.get('CXX', env['CXX'])
)
