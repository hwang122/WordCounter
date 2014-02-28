CC=g++
CXXFLAGS = -std=c++0x -c -g

all: WordCounter_small WordCounter_large

WordCounter_small: WordCounter_small.o
	$(CC) -o WordCounter_small WordCounter_small.o -lpthread

WordCounter_large: WordCounter_large.o
	$(CC) -o WordCounter_large WordCounter_large.o -lpthread

WordCounter_small.o: WordCounter_small.cpp
	$(CC) $(CXXFLAGS) WordCounter_small.cpp

WordCounter_large.o: WordCounter_large.cpp
	$(CC) $(CXXFLAGS) WordCounter_large.cpp

clean:
	rm WordCounter_small.o WordCounter_small WordCounter_large.o \
	WordCounter_large