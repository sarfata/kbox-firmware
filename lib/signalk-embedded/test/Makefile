# signalk Library tests
#
# This Makefile runs unit tests on the host computer (Linux/macOS).
#

# Build flags - Including all dependencies of the test
CFLAGS := -g -Wall
CFLAGS := $(CFLAGS) -I../src
CPPFLAGS := $(CFLAGS) -std=c++11
CFLAGS := $(CFLAGS) -std=c11

# Transformation rules for C and CPP files
%.o: %.cpp Makefile
	$(CXX) $(CPPFLAGS) -c $< -o $@

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

all: test

test: signalk-test skkv-test signalk-path-test
	./signalk-path-test
	./skkv-test
	./signalk-test

# Object dependencies of the tests. Without them tests cannot run.
TEST_DEPS=maintest.o

signalk-path-test: signalk-path-test.o ../src/signalk-path.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

skkv-test: skkv-test.o ../src/skkv.o ../src/signalk.o ../src/signalk-path.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

signalk-test: signalk-test.o ../src/signalk.o ../src/skkv.o ../src/signalk-path.o $(TEST_DEPS)
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	rm -f signalk-test
	find .. -name '*.o' |xargs rm -v

