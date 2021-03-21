LDLIBS=-lasound -lm

all: int8 sin aduplex

int8: int8.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

sin: sin.o
	$(CC) $(LDFLAGS) $(LDLIBS) $^ -o $@

aduplex: aduplex.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $^ -o $@
	
aduplex.o: aduplex.cpp
	$(CXX) -c $<

clean:
	rm -f int8.o sin.o aduplex.o
	rm -f int8 sin aduplex

