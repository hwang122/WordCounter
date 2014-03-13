#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simpleHashTable.h"

#define BUFFER_SIZE		SIZE * 6

//global function callable from main function
//count each word's occurrence
__global__ void counter(unsigned int *key, Table table, Lock *lock, int count)
{
	int tid = threadIdx.x + blockIdx.x * blockDim.x;
    int stride = blockDim.x * gridDim.x;
	
	/** initialize table's pool in the initialize table funtion
		make sure zero out values could only occurs once
	//initialize table
	if(tid == 0)
	{
		printf("In zero out table\n");
		zero_out_values_in_table(table);
		//need to be synchronized
		__syncthreads();
	}
	**/
	//all the key with index larger than count is not exist
    while (tid < count) {
		unsigned int k = key[tid];
		put(table, k, lock, tid);
        tid += stride;
    }
}

int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		printf("Usage: %s <File name> <Searching word> <Number of threads>",
				argv[0]);
		exit(-1);
	}
	char *filename = argv[1];
	unsigned int key_num = (unsigned int)atoi(argv[2]);
	int numThread = atoi(argv[3]);
	
	//open file
	FILE *pFile;
	pFile = fopen(filename, "r");
	if(pFile == NULL)
	{
		printf("Fail to open file");
		exit(-1);
	}
	//time function
	cudaEvent_t start, stop;
    HANDLE_ERROR(cudaEventCreate(&start));
    HANDLE_ERROR(cudaEventCreate(&stop));
    HANDLE_ERROR(cudaEventRecord(start, 0));
	
	//copy hash table and lock to GPU
	Table dev_table;
    initialize_table(dev_table, HASH_ENTRIES, ELEMENTS);

    Lock lock[HASH_ENTRIES];
    Lock *dev_lock;
    HANDLE_ERROR(cudaMalloc((void**)&dev_lock, HASH_ENTRIES * sizeof( Lock)));
    HANDLE_ERROR(cudaMemcpy(dev_lock, lock, HASH_ENTRIES * sizeof( Lock ),	
                              cudaMemcpyHostToDevice ) );
	
	/**
	//set count in GPU to count number of elements
	unsigned int count = 0, *dev_count;
	cudaMalloc((void**)&dev_count, sizeof(unsigned int));
	HANDLE_ERROR(cudaMemcpy(dev_count, &count, sizeof(unsigned int), 
							cudaMemcpyHostToDevice));
	**/
	
	//set block number and thread number per block
	int numBlock = 32;
	int threadPerBlock = numThread/numBlock;
	//copy file to GPU
	unsigned int *key, *d_key;
	while(!feof(pFile))
	{
		key = (unsigned int*)malloc(BUFFER_SIZE);
		int num = BUFFER_SIZE/sizeof(unsigned int);
		
		int i, count;
		for(i = 0; i < num && !feof(pFile); i++)
		{
			fscanf(pFile, "%u", &key[i]);
			count = i;
		}
		
		HANDLE_ERROR(cudaMalloc((void**)&d_key, BUFFER_SIZE));
		HANDLE_ERROR(cudaMemcpy(d_key, key, BUFFER_SIZE, cudaMemcpyHostToDevice));

		//count the word on GPU
		counter<<<numBlock, threadPerBlock>>>(d_key, dev_table, dev_lock, count+1);
		//wait until all device ended
		cudaDeviceSynchronize();
		//free memory in GPU
		HANDLE_ERROR(cudaFree(d_key));
		free(key);
	}
	
	//copy hash table from device to host
	Table table;
	copy_table_to_host(dev_table, table);
	unsigned long occurrence = 0;
	occurrence = get(table, key_num);
	if(occurrence == 0)
		printf("Key number not found!");
	else
		printf("Occurrence of Key number: %u\n", occurrence);
	
	HANDLE_ERROR(cudaEventRecord(stop, 0));
    HANDLE_ERROR(cudaEventSynchronize(stop));
    float elapsedTime;
    HANDLE_ERROR(cudaEventElapsedTime(&elapsedTime, start, stop));
    printf("Time to hash:  %3.1f ms\n", elapsedTime);
	
	free(table.pool);
	free(table.entries);
	HANDLE_ERROR(cudaEventDestroy(start));
    HANDLE_ERROR(cudaEventDestroy(stop));
    free_table(dev_table);
    HANDLE_ERROR(cudaFree(dev_lock));
	return 0;
}

