FRAMEWORK=$(HOME)/.platformio/packages/framework-arduinoteensy/cores/teensy3
CFLAGS=-g -Wall -Iarduinomock/ --I$(FRAMEWORK) -I../../KBoxDebug/src -I../../List/src -I../../NMEA2000 -I../src -D_NEWLIB_VERSION=1
CPPFLAGS=$(CFLAGS) -std=c++11

%.o: %.cpp Makefile
	$(CC) $(CPPFLAGS) -c $< -o $@

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

all: test

test: nmea_test slip_test NMEASentenceBuilderTest KMessageNMEAVisitorTest
	./nmea_test
	./slip_test
	./NMEASentenceBuilderTest
	./KMessageNMEAVisitorTest


TEST_DEPS=teensy_compat.o maintest.o $(FRAMEWORK)/WString.o $(FRAMEWORK)/Print.o

nmea_test: nmea_test.o ../src/util/nmea.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

slip_test: slip_test.cpp ../src/util/SlipStream.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

NMEASentenceBuilderTest: NMEASentenceBuilderTest.o ../src/util/NMEASentenceBuilder.o ../src/util/nmea.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

KMessageNMEAVisitorTest: KMessageNMEAVisitorTest.o ../src/util/NMEASentenceBuilder.o ../src/KMessage.o ../src/util/nmea.o ../src/KMessageNMEAVisitor.o ../../NMEA2000/N2kMsg.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	rm -f nmea_test slip_test NMEASentenceBuilderTest
	find .. -name '*.o' |xargs rm -v

