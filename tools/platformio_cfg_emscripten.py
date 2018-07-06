Import("env")
from sys import platform

env.Replace(
    PROGNAME="sktool.js",
    PROGSUFFIX=".js",
    CC="emcc",
    CXX="em++",
    LINKFLAGS="-s EXPORTED_FUNCTIONS='[\"_sktoolConvert\"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\", \"cwrap\"]'"
)
