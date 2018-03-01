CXX	 = g++
CXXFLAGS = -pedantic -Wall -std=c++11 -Wextra -I./include -lpthread -O3
SRC	 = src/
INC	 = include/
BIN	 = bin/

.PHONY = clean

all: stats

stats: stats.o statsgen.o
	$(CXX) $^ $(CXXFLAGS) -o $@

stats.o: $(SRC)main.cpp $(INC)statsgen.h
	$(CXX) $(CXXFLAGS) $(SRC)main.cpp -c -o $@

statsgen.o: $(SRC)statsgen.cpp $(INC)statsgen.h
	$(CXX) $(CXXFLAGS) $(SRC)statsgen.cpp -c -o $@

clean:
	-rm -f *.o stats