

all: first-stage addup

first-stage: first-stage.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@
	
addup: addup.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

clean:
	rm -f first-stage.o addup.o
	rm -f first-stage addup


