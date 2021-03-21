LDLIBS=-lasound -lm

all: int8 sin aduplex dc dec8 int8

sin: sin.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

dc: dc.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

dec8: dec8.o prbs1bit.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

int8: int8.o prbs1bit.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

aduplex: aduplex.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

aduplex.o: aduplex.cpp
	$(CXX) -c $<

prbs1bit.o: prbs1bit.cpp
	$(CXX) -c $<

clean:
	rm -f int8.o sin.o aduplex.o dc.o prbs1bit.o dec8.o int8.o
	rm -f int8 sin aduplex dc dec8 int8

