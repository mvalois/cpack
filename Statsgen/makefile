CXX	 = g++
CXXFLAGS = -pedantic -Wall -std=c++11 -Wextra -Iinclude -O3
LDFLAGS  = -lpthread
SRC	 = src
INC	 = include
BIN  = cstatsgen
BINGUI = cstatsgen-gui

.PHONY = clean

all: $(BIN) $(BINGUI)

$(BIN): main.o statsgen.o
	$(CXX) $^ -o $@ $(LDFLAGS)

main.o: $(SRC)/cli/main.cpp $(INC)/statsgen.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

statsgen.o: $(SRC)/core/statsgen.cpp $(INC)/statsgen.h $(INC)/utils.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

clean:
	-rm -f *.o stats

$(BINGUI): statsgen.o
	qmake -o qmakefile
	make -f qmakefile
