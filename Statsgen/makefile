CXX	 = g++
CXXFLAGS = -pedantic -Wall -std=c++11 -Wextra -I./include -lpthread
SRC	 = src/
INC	 = include/
BIN	 = bin/



.PHONY 	 = stats clean


all: stats


stats: $(BIN)stats.o $(BIN)statsgen.o
	$(CXX) $^ $(CXXFLAGS) -o $@



$(BIN)stats.o: $(SRC)main.cpp $(INC)statsgen.h
	$(CXX) $(CXXFLAGS) $(SRC)main.cpp -c -o $@

$(BIN)statsgen.o: $(SRC)statsgen.cpp $(INC)statsgen.h
	$(CXX) $(CXXFLAGS) $(SRC)statsgen.cpp -c -o $@



clean:
	rm $(BIN)/*.o