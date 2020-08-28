

all: first-stage addup prbs

first-stage: first-stage.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@
	
addup: addup.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@
	
prbs: prbs.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

clean:
	rm -f first-stage.o addup.o prbs.o
	rm -f first-stage addup prbs


