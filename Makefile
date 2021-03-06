RM := rm -f

SRC := $(wildcard *.cpp)
BIN := $(patsubst %.cpp,%,$(SRC))

CXXFLAGS += -pedantic -Wall
ifdef DEBUG
CXXFLAGS += -g -Og
else
CXXFLAGS += -O3
endif

.PHONY: all clean

all: $(BIN)

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	$(RM) $(BIN)
