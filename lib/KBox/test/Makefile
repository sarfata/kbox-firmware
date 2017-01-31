# KBox Library tests
#
# This Makefile runs unit tests on the host computer (Linux/macOS). It tries
# to emulate the Arduino environment so the files can compile and run.
#

# Location of the Teensy framework source
FRAMEWORK := $(HOME)/.platformio/packages/framework-arduinoteensy/cores/teensy3

# Build flags - Including all dependencies of the test
CFLAGS := -g -Wall
CFLAGS := $(CFLAGS) -Iarduinomock/ -I$(FRAMEWORK) -I../../KBoxDebug/src
CFLAGS := $(CFLAGS) -I../../List/src -I../../NMEA2000 -I../src
CPPFLAGS := $(CFLAGS) -std=c++11 -DARDUINO

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
nmea_test: nmea_test.o ../src/util/nmea.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

# Test the SLIP encapsulation code
slip_test: slip_test.o ../src/util/SlipStream.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

# Test the NMEASentenceBuilder
NMEASentenceBuilderTest: NMEASentenceBuilderTest.o ../src/util/NMEASentenceBuilder.o ../src/util/nmea.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

# Test the visitor that converts KMessage into NMEA
KMessageNMEAVisitorTest: KMessageNMEAVisitorTest.o ../src/util/NMEASentenceBuilder.o ../src/KMessage.o ../src/util/nmea.o ../src/util/nmea2000.o ../src/KMessageNMEAVisitor.o ../../NMEA2000/N2kMsg.o ../../NMEA2000/N2kStream.cpp $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	rm -f nmea_test slip_test NMEASentenceBuilderTest
	find .. -name '*.o' |xargs rm -v

