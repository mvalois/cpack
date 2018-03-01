CXX	 = g++
CXXFLAGS = -pedantic -Wall -std=c++11 -Wextra -Iinclude -O3
LDFLAGS  = -lpthread
SRC	 = src
INC	 = include
BIN  = cstatsgen

.PHONY = clean

$(BIN): main.o statsgen.o
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: $(SRC)/%.cpp $(INC)/statsgen.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

clean:
	-rm -f *.o stats