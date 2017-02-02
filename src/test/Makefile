# KBox Library tests
#
# This Makefile runs unit tests on the host computer (Linux/macOS). It tries
# to emulate the Arduino environment so the files can compile and run.
#

# Location of the Teensy framework source
FRAMEWORK := $(HOME)/.platformio/packages/framework-arduinoteensy/cores/teensy3

# Build flags - Including all dependencies of the test
CFLAGS := -g -Wall
CFLAGS := $(CFLAGS) -Iarduinomock/ -I$(FRAMEWORK) -I../../lib/KBoxLogging/src
CFLAGS := $(CFLAGS) -I../../lib/List/src -I../../lib/NMEA2000 -I../common
CPPFLAGS := $(CFLAGS) -std=c++11

# Transformation rules for C and CPP files
%.o: %.cpp Makefile
	$(CXX) $(CPPFLAGS) -c $< -o $@

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

all: test

test: nmea_test slip_test NMEASentenceBuilderTest KMessageNMEAVisitorTest
	./nmea_test
	./slip_test
	./NMEASentenceBuilderTest
	./KMessageNMEAVisitorTest

# Object dependencies of the tests. Without them tests cannot run.
TEST_DEPS=teensy_compat.o maintest.o $(FRAMEWORK)/WString.o $(FRAMEWORK)/Print.o

$(FRAMEWORK)/WString.o: $(FRAMEWORK)/WString.cpp
	# Add the _NEWLIB_VERSION define when building WString to include
	# itoa, utoa, etc (see avr_functions.h in the teensy framework).
	$(CXX) $(CPPFLAGS) -D_NEWLIB_VERSION=1 -c $^ -o $@

# Test the NMEA helper functions
nmea_test: nmea_test.o ../common/nmea/nmea.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

# Test the SLIP encapsulation code
slip_test: slip_test.o ../common/comms/SlipStream.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

# Test the NMEASentenceBuilder
NMEASentenceBuilderTest: NMEASentenceBuilderTest.o ../common/nmea/NMEASentenceBuilder.o ../common/nmea/nmea.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

# Test the visitor that converts KMessage into NMEA
KMessageNMEAVisitorTest: KMessageNMEAVisitorTest.o ../common/nmea/NMEASentenceBuilder.o ../common/signalk/KMessage.o ../common/nmea/nmea.o ../common/nmea/nmea2000.o ../src/signalk/KMessageNMEAVisitor.o ../../NMEA2000/N2kMsg.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	rm -f nmea_test slip_test NMEASentenceBuilderTest KMessageNMEAVisitorTest
	find .. -name '*.o' |xargs rm -v

