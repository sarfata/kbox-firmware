FRAMEWORK=$(HOME)/.platformio/packages/framework-arduinoteensy/cores/teensy3
CFLAGS=-g -Wall -std=c++11 -I$(FRAMEWORK) -I../../KBoxDebug/src -I../src -D_NEWLIB_VERSION=1

all: test

test: nmea_test slip_test NMEASentenceBuilder
	./nmea_test
	./slip_test
	./NMEASentenceBuilder

nmea_test: nmea_test.cpp ../src/util/nmea.h ../src/util/nmea.c
	$(CXX) $(CFLAGS) nmea_test.cpp ../src/util/nmea.c -o nmea_test

slip_test: slip_test.cpp ../src/util/SlipStream.h ../src/util/SlipStream.cpp
	$(CXX) $(CFLAGS) slip_test.cpp ../src/util/SlipStream.cpp $(FRAMEWORK)/Print.cpp -o slip_test

NMEASentenceBuilder: NMEASentenceBuilderTest.cpp ../src/util/NMEASentenceBuilder.h ../src/util/NMEASentenceBuilder.cpp ../src/util/nmea.h ../src/util/nmea.c teensy_compat.c
	$(CXX) $(CFLAGS) NMEASentenceBuilderTest.cpp ../src/util/NMEASentenceBuilder.cpp $(FRAMEWORK)/WString.cpp teensy_compat.c ../src/util/nmea.c -o NMEASentenceBuilder

clean:
	rm -f nmea_test slip_test
