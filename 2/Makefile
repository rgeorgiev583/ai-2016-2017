MKDIR := mkdir -p
RM := rm -rf

BINDIR := bin
OBJDIR := obj

BIN := $(BINDIR)/astar
SRC := $(wildcard *.cpp)
OBJ := $(addprefix $(OBJDIR)/,$(patsubst %.cpp,%.o,$(SRC)))
CH := $(wildcard *.gch)
ifdef DEBUG
CXXFLAGS += -g -Og
else
CXXFLAGS += -O3
endif

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ) $(BINDIR)
	$(CXX) $(LDFLAGS) $(OBJ) -o $@

$(OBJDIR)/%.o: %.cpp $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BINDIR):
	$(MKDIR) $@

$(OBJDIR):
	$(MKDIR) $@

clean:
	$(RM) $(CH) $(OBJDIR) $(BINDIR)
