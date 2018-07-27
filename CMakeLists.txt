cmake_minimum_required(VERSION 3.2)
project(kbox-firmware)

# include_directories("$ENV{HOME}/.platformio/packages/framework-arduinoteensy/libraries/Time")
#include_directories("$ENV{HOME}/work/kbox-firmware/.piolibdeps/Time_ID44")

#include_directories("$ENV{HOME}/.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include")
#include_directories("$ENV{HOME}/.platformio/packages/toolchain-gccarmnoneeabi/lib/gcc/arm-none-eabi/5.4.1/include")
#include_directories("$ENV{HOME}/.platformio/packages/toolchain-gccarmnoneeabi/lib/gcc/arm-none-eabi/5.4.1/include-fixed")

# This needs to be replaced by a toolchain file
# https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/CrossCompiling
#SET(CMAKE_C_COMPILER "/Users/thomas/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc")
#SET(CMAKE_CXX_COMPILER "/Users/thomas/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++")


# Options I removed for x86:
# removed for x86 -mthumb -mcpu=cortex-m4 -nostdlib

# Probably should replace all of these by add_definitions / add_options instead
SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-fno-exceptions -felide-constructors -fno-rtti -std=gnu++14 -g -Os -Wall
    -ffunction-sections -fdata-sections -fsingle-precision-constant -fstack-usage -Wall -Werror
    -DKBOX_VERSION=&quot;test&quot; -DF_CPU=72000000L
    -DLAYOUT_US_ENGLISH -DPLATFORMIO=30401 -D__MK20DX256__ -DTEENSY31
    -DSERIAL1_RX_BUFFER_SIZE=512 -DSERIAL1_TX_BUFFER_SIZE=512 -DUSB_SERIAL -DARDUINO=10805 -DTEENSYDUINO=140")
SET(CMAKE_C_FLAGS_DISTRIBUTION "-g -Os -Wall
    -ffunction-sections -fdata-sections -fsingle-precision-constant -fstack-usage -Wall -Werror
    -DKBOX_VERSION=&quot;test&quot; -DF_CPU=72000000L
     -DLAYOUT_US_ENGLISH -DPLATFORMIO=30401 -D__MK20DX256__ -DTEENSY31
     -DSERIAL1_RX_BUFFER_SIZE=512 -DSERIAL1_TX_BUFFER_SIZE=512 -DUSB_SERIAL -DARDUINO=10805 -DTEENSYDUINO=140")


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE off)

#add_definitions(-DF_CPU=72000000L)
#add_definitions(-DLAYOUT_US_ENGLISH)
#add_definitions(-DPLATFORMIO=30401)
#add_definitions(-D__MK20DX256__)
#add_definitions(-DTEENSY31)
#add_definitions(-DSERIAL1_RX_BUFFER_SIZE=512)
#add_definitions(-DSERIAL1_TX_BUFFER_SIZE=512)
#add_definitions(-DUSB_SERIAL)
#add_definitions(-DARDUINO=10805)
#add_definitions(-DTEENSYDUINO=140)

#include(CMakeListsPrivate.txt)






FILE(GLOB_RECURSE TEST_LIST
        "/Users/thomas/work/kbox-firmware/.piolibdeps/NMEA2000-library/src/*.*"
        "/Users/thomas/work/kbox-firmware/lib/KBoxLogging/src/*.*"
        "/Users/thomas/work/kbox-firmware/src/common/comms/*.*"
        "/Users/thomas/work/kbox-firmware/src/common/nmea/*.*"
        "/Users/thomas/work/kbox-firmware/src/common/signalk/*.*"
        "/Users/thomas/work/kbox-firmware/src/common/time/*.*"
        "/Users/thomas/work/kbox-firmware/src/common/util/*.*"
        "/Users/thomas/work/kbox-firmware/src/host/config/*.*"
        "/Users/thomas/work/kbox-firmware/src/test/*.*"
        )
add_executable(testrunner ${TEST_LIST})
target_compile_options(testrunner PRIVATE -g -O0)
target_compile_definitions(testrunner PRIVATE -DKBOX_TESTS -DHAVE_STRLCAT -DHAVE_STRLCPY)
target_include_directories(testrunner PRIVATE
        src
        .piolibdeps/NMEA2000-library/src
        .piolibdeps/ArduinoJson/src
        lib/KBoxLogging/src
        .piolibdeps/elapsedMillis_ID1002
        src/test/arduinomock
        src/test/teensyheaders
        )

enable_testing()
add_test(testrunner testrunner)


# These targets make it possible to run platformio from various IDEs

set(PLATFORMIO_CMD "/usr/local/bin/platformio")

add_custom_target(
    KBOX_TEST
    COMMAND ${PLATFORMIO_CMD} -f -c clion run -e test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND .pioenvs/test/program
)

add_custom_target(
    PLATFORMIO_BUILD_HOST
    COMMAND ${PLATFORMIO_CMD} -f -c clion run -e host
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_BUILD_ESP
    COMMAND ${PLATFORMIO_CMD} -f -c clion run -e esp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

FILE(GLOB_RECURSE SRC_LIST "")
add_executable(${PROJECT_NAME} /Users/thomas/work/kbox-firmware/src/host/main.cpp)

