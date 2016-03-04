all: test

test: runtest
	./runtest

runtest: test.cpp ../src/List.h
	g++ -g -Wall -I ../src test.cpp -o runtest
