CXX	 = g++
CXXFLAGS = -pedantic -Wall -std=c++11 -Wextra -I./include -lpthread
SRC	 = src/
INC	 = include/
BIN	 = bin/



.PHONY 	 = Stats clean


all: Stats


Stats: $(BIN)Stats.o $(BIN)statsgen.o
	$(CXX) $^ $(CXXFLAGS) -o $@



$(BIN)Stats.o: $(SRC)main.cpp $(INC)statsgen.h
	$(CXX) $(CXXFLAGS) $(SRC)main.cpp -c -o $@

$(BIN)statsgen.o: $(SRC)statsgen.cpp $(INC)statsgen.h
	$(CXX) $(CXXFLAGS) $(SRC)statsgen.cpp -c -o $@



clean:
	rm $(BIN)/*.o