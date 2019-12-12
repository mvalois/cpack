CXX	 = g++
CXXFLAGS = -pedantic -Wall -std=c++11 -Wextra -Iinclude -O3
LDFLAGS  = -lpthread
SRC	 = src
INC	 = include
OBJ = obj
BIN  = cppack
BINGUI = $(BIN)-gui

.PHONY = clean

all: $(BIN) $(BINGUI)

$(BIN): $(OBJ)/main.o $(OBJ)/statsgen.o
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJ)/main.o: $(SRC)/cli/main.cpp $(INC)/statsgen.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(OBJ)/statsgen.o: $(SRC)/core/statsgen.cpp $(INC)/statsgen.h $(INC)/utils.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

clean:
	-rm -rf $(OBJ)/* $(BIN) $(BINGUI)

$(BINGUI):
	qmake -o qmakefile
	$(MAKE) -f qmakefile
