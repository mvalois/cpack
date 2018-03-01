CXX	 = g++
CXXFLAGS = -pedantic -Wall -std=c++11 -Wextra -Iinclude -lpthread -O3
SRC	 = src
INC	 = include
BIN	 = bin

.PHONY = clean

stats: main.o statsgen.o
	$(CXX) $^ $(CXXFLAGS) -o $@

%.o: $(SRC)/%.cpp $(INC)/statsgen.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

clean:
	-rm -f *.o stats