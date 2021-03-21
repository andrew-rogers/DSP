LDLIBS=-lasound

all: int8

int8: int8.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

clean:
	rm -f int8.o
	rm -f int8

