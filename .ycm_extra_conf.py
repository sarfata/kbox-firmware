#!/usr/bin/env python
#
# Copyright (C) 2014  Google Inc.
#
# This file is part of YouCompleteMe.
#
# YouCompleteMe is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# YouCompleteMe is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with YouCompleteMe.  If not, see <http://www.gnu.org/licenses/>.

import os
import ycm_core
import logging

# Logger for additional logging.
# To enable debug logging, add `let g:ycm_server_log_level = 'debug'` to
# your .vimrc file.
logger = logging.getLogger('ycm-extra-conf')


# Platformio Autogen libs.
## Platformio automatically copies over the libs you use after your first run.
## Be warned that you will not receive autocompletion on libraries until after
## your first `platformio run`.
PlatformioAutogen = ".piolibdeps/"

# Platformio Arduino Std Libs
## Arduino Std libs from .platformio packages. Provides stdlib.h and such.
PlatformioArduinoSTD = '~/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include/'

PlatformTeensy = '~/.platformio/packages/framework-arduinoteensy/cores/teensy3/'
PlatformESP8266 = '~/.platformio/packages/framework-arduinoespressif8266/cores/esp8266/'

# This is the list of all directories to search for header files.
# Dirs in this list can be paths relative to this file, absolute
# paths, or paths relative to the user (using ~/path/to/file).
# There is really no way to know when editing files which framework they will
# get compiled against so we include both the teensy and the esp8266 framework.
libDirs = [
           "lib"
           ,PlatformioAutogen
           ,PlatformioArduinoSTD
           ,PlatformTeensy
           ,PlatformESP8266
           ]


# These are the compilation flags that will be used in case there's no
# compilation database set (by default, one is not set).
# CHANGE THIS LIST OF FLAGS. YES, THIS IS THE DROID YOU HAVE BEEN LOOKING FOR.
flags = [   '-std=c++11',
            '-fpermissive',
            '-nostdlib',
            '-MMD',
            '-DF_CPU=72000000L',
            '-DUSB_SERIAL',
            '-DLAYOUT_US_ENGLISH',
            '-D__MK20DX256__',
            '-DARDUINO=10600',
            '-DTEENSYDUINO=124',
            '-DPLATFORMIO=020401',
            '-Isrc/'
]

# Set this to the absolute path to the folder (NOT the file!) containing the
# compile_commands.json file to use that instead of 'flags'. See here for
# more details: http://clang.llvm.org/docs/JSONCompilationDatabase.html
#
# Most projects will NOT need to set this to anything; you can just change the
# 'flags' list of compilation flags.
compilation_database_folder = ''

if os.path.exists( compilation_database_folder ):
  database = ycm_core.CompilationDatabase( compilation_database_folder )
else:
  database = None

SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', '.m', '.mm' ]

def DirectoryOfThisScript():
  return os.path.dirname( os.path.abspath( __file__ ) )


def MakeRelativePathsInFlagsAbsolute( flags, working_directory ):
  if not working_directory:
    return list( flags )
  new_flags = []
  make_next_absolute = False
  path_flags = [ '-isystem', '-I', '-iquote', '--sysroot=' ]

  for libDir in libDirs:

    # dir is relative to $HOME
    if libDir.startswith('~'):
        libDir = os.path.expanduser(libDir)

    # dir is relative to `working_directory`
    if not libDir.startswith('/'):
        libDir = os.path.join(working_directory,libDir)

    # Else, assume dir is absolute

    for path, dirs, files in os.walk(libDir):
        # Add to flags if dir contains a header file and is not
        # one of the metadata dirs (examples and extras).
        if any(IsHeaderFile(x) for x in files) and\
          path.find("examples") is -1 and path.find("extras") is -1:
            logger.debug("Directory contains header files - %s"%path)
            flags.append('-I'+path)

  for flag in flags:
    new_flag = flag

    if make_next_absolute:
      make_next_absolute = False
      if not flag.startswith( '/' ):
        new_flag = os.path.join( working_directory, flag )

    for path_flag in path_flags:
      if flag == path_flag:
        make_next_absolute = True
        break

      if flag.startswith( path_flag ):
        path = flag[ len( path_flag ): ]
        new_flag = path_flag + os.path.join( working_directory, path )
        break

    if new_flag:
      new_flags.append( new_flag )
  return new_flags


def IsHeaderFile( filename ):
  extension = os.path.splitext( filename )[ 1 ]
  return extension in [ '.h', '.hxx', '.hpp', '.hh' ]


def GetCompilationInfoForFile( filename ):
  # The compilation_commands.json file generated by CMake does not have entries
  # for header files. So we do our best by asking the db for flags for a
  # corresponding source file, if any. If one exists, the flags for that file
  # should be good enough.
  if IsHeaderFile( filename ):
    basename = os.path.splitext( filename )[ 0 ]
    for extension in SOURCE_EXTENSIONS:
      replacement_file = basename + extension
      if os.path.exists( replacement_file ):
        compilation_info = database.GetCompilationInfoForFile(
          replacement_file )
        if compilation_info.compiler_flags_:
          return compilation_info
    return None
  return database.GetCompilationInfoForFile( filename )


# This is the entry point; this function is called by ycmd to produce flags for
# a file.
def FlagsForFile( filename, **kwargs ):
  if database:
    # Bear in mind that compilation_info.compiler_flags_ does NOT return a
    # python list, but a "list-like" StringVec object
    compilation_info = GetCompilationInfoForFile( filename )
    if not compilation_info:
      return None

    final_flags = MakeRelativePathsInFlagsAbsolute(
      compilation_info.compiler_flags_,
      compilation_info.compiler_working_dir_ )
  else:
    relative_to = DirectoryOfThisScript()
    final_flags = MakeRelativePathsInFlagsAbsolute( flags, relative_to )

  return {
    'flags': final_flags,
    'do_cache': True
  }

