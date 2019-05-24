OBJS=test.o DuplexAudio.o

all: test

test.o: test.cpp DuplexAudio.h
	g++ -c $< -fpermissive

DuplexAudio.o: DuplexAudio.cpp DuplexAudio.h
	g++ -c $< -fpermissive

test: $(OBJS)
	g++ $^ -o $@ -lportaudio -lm

clean:
	rm -f $(OBJS)
	rm -f test
