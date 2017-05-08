from SCons.Script import DefaultEnvironment
import os

env = DefaultEnvironment()

print "Default CXX {}  Environ set CXX {}".format(env['CXX'], os.environ.get('CXX'))

env.Replace(
    CC=os.environ.get('CC', env['CC']),
    CXX=os.environ.get('CXX', env['CXX'])
)
