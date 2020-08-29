LDLIBS=-lasound

all: first-stage addup prbs acapture aplay aduplex

first-stage: first-stage.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@
	
addup: addup.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@
	
prbs: prbs.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@
	
acapture: acapture.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

aplay: aplay.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

aduplex: aduplex.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

clean:
	rm -f first-stage.o addup.o prbs.o acapture.o aplay.o aduplex.o
	rm -f first-stage addup prbs acapture aplay aduplex


