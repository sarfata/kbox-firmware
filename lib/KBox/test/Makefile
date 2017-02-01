# KBox Library tests
#
# This Makefile runs unit tests on the host computer (Linux/macOS). It tries
# to emulate the Arduino environment so the files can compile and run.
#

# Location of the Teensy framework source
FRAMEWORK := $(HOME)/.platformio/packages/framework-arduinoteensy/cores/teensy3

# Build flags - Including all dependencies of the test
CFLAGS := -g -Wall
CFLAGS := $(CFLAGS) -Iarduinomock/ -I$(FRAMEWORK) -I../../KBoxLogging/src
CFLAGS := $(CFLAGS) -I../../List/src -I../../NMEA2000 -I../src
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
nmea_test: nmea_test.o ../src/nmea/nmea.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

# Test the SLIP encapsulation code
slip_test: slip_test.o ../src/comms/SlipStream.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

# Test the NMEASentenceBuilder
NMEASentenceBuilderTest: NMEASentenceBuilderTest.o ../src/nmea/NMEASentenceBuilder.o ../src/nmea/nmea.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

# Test the visitor that converts KMessage into NMEA
KMessageNMEAVisitorTest: KMessageNMEAVisitorTest.o ../src/nmea/NMEASentenceBuilder.o ../src/signalk/KMessage.o ../src/nmea/nmea.o ../src/nmea/nmea2000.o ../src/signalk/KMessageNMEAVisitor.o ../../NMEA2000/N2kMsg.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	rm -f nmea_test slip_test NMEASentenceBuilderTest KMessageNMEAVisitorTest
	find .. -name '*.o' |xargs rm -v

