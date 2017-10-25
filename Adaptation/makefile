CXX	 = g++
CXXFLAGS = -pedantic -Wall -std=c++11 -Wextra -I./include
SRC	 = src/
INC	 = include/



.PHONY 	 = main clean


all: main clean


main: main.o statsgen.o
	$(CXX) $(CXXFLAGS) $^ -o $@



main.o: $(SRC)main.cpp $(INC)statsgen.h
	$(CXX) $(CXXFLAGS) $(SRC)main.cpp -c -o $@

statsgen.o: $(SRC)statsgen.cpp $(INC)statsgen.h
	$(CXX) $(CXXFLAGS) $(SRC)statsgen.cpp -c -o $@



clean:
	rm *.o