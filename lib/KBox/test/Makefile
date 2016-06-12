FRAMEWORK=$(HOME)/.platformio/packages/framework-arduinoteensy/cores/teensy3
CFLAGS=-g -Wall -std=c++11 -I$(FRAMEWORK) -I../src

all: test

test: nmea_test slip_test
	./nmea_test
	./slip_test

nmea_test: nmea_test.cpp ../src/util/nmea.h ../src/util/nmea.c
	g++ -g -Wall -I ../src nmea_test.cpp ../src/util/nmea.c -o nmea_test

slip_test: slip_test.cpp ../src/util/SlipStream.h ../src/util/SlipStream.cpp 
	g++ $(CFLAGS) slip_test.cpp ../src/util/SlipStream.cpp $(FRAMEWORK)/Print.cpp -o slip_test
