all: test

test: nmea_test
	./nmea_test

nmea_test: nmea_test.cpp ../src/util/nmea.h ../src/util/nmea.c
	g++ -g -Wall -I ../src nmea_test.cpp ../src/util/nmea.c -o nmea_test
