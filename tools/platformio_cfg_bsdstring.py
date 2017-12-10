from SCons.Script import DefaultEnvironment
from sys import platform

env = DefaultEnvironment()

if platform == "darwin":
    env.Append(CCFLAGS=["-DHAVE_STRLCPY", "-DHAVE_STRLCAT"])

