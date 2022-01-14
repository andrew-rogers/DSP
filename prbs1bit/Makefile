LDLIBS=-lasound -lm

all: dec8 int8 interpolate sin dc aduplex duplex8 duplex

dec8: dec8.o prbs1bit.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

int8: int8.o prbs1bit.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

interpolate: interpolate.o prbs1bit.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

sin: sin.o prbs1bit.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

dc: dc.o prbs1bit.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

aduplex: aduplex.o ALSADuplex.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

duplex8: duplex8.o ALSADuplex.o prbs1bit.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

duplex: duplex.o ALSADuplex.o prbs1bit.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@

aduplex.o: aduplex.cpp
	$(CXX) -c $<

prbs1bit.o: prbs1bit.cpp
	$(CXX) -c $<

ALSADuplex.o: ALSADuplex.cpp
	$(CXX) -c $<

clean:
	rm -f int8.o interpolate.o sin.o aduplex.o dc.o prbs1bit.o dec8.o int8.o ALSADuplex.o duplex8.o duplex.o
	rm -f int8 interpolate sin aduplex dc dec8 int8 duplex8 duplex

