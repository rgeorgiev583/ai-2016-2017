PHONY := all clean

all: $(wildcard *.out)

%: %.out

%.out: %.cpp
	$(CC) $(CFLAGS) -g $< -o $@

clean:
	rm -f *.out
