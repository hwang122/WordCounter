CC=nvcc
CXXFLAGS=-arch sm_20 -G -g
OBJ=WordCounter_CUDA.o simpleHashTable.o

all: WordCounter_CUDA

WordCounter_CUDA: $(OBJ)
	$(CC) $(CXXFLAGS) -o WordCounter_CUDA $(OBJ)

WordCounter_CUDA.o: WordCounter_CUDA.cu
	$(CC) $(CXXFLAGS) -dc WordCounter_CUDA.cu

simpleHashTable.o: simpleHashTable.cu
	$(CC) $(CXXFLAGS) -dc simpleHashTable.cu

.PHONY: clean
clean:
	-rm $(OBJ) WordCounter_CUDA