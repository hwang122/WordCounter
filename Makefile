CC=g++
CXXFLAGS = -std=c++0x -c -g

all: WordCounter_small WordCounter_large WordCounter_large_2

WordCounter_small: WordCounter_small.o
	$(CC) -o WordCounter_small WordCounter_small.o -lpthread

WordCounter_large: WordCounter_large.o Mstrtok.o
	$(CC) -o WordCounter_large WordCounter_large.o Mstrtok.o -lpthread

WordCounter_large_2: WordCounter_large_2.o
	$(CC) -o WordCounter_large_2 WordCounter_large_2.o -lpthread

WordCounter_small.o: WordCounter_small.cpp
	$(CC) $(CXXFLAGS) WordCounter_small.cpp

WordCounter_large.o: WordCounter_large.cpp
	$(CC) $(CXXFLAGS) WordCounter_large.cpp

WordCounter_large_2.o: WordCounter_large_2.cpp
	$(CC) $(CXXFLAGS) WordCounter_large_2.cpp

Mstrtok.o: Mstrtok.cpp Mstrtok.h
	$(CC) -c Mstrtok.cpp

clean:
	rm WordCounter_small.o WordCounter_small WordCounter_large.o \
	WordCounter_large Mstrtok.o WordCounter_large_2 WordCounter_large_2.o