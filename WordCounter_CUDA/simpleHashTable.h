#include "book.h"
#include "lock.h"

#ifndef __SIMPLEHASHTABLE_H__
#define __SIMPLEHASHTABLE_H__

#define SIZE    (100*1024*1024)
#define ELEMENTS    (SIZE / sizeof(unsigned int))
#define HASH_ENTRIES     10240


// an Entry contains a key and a value
struct Entry {
  unsigned int    key;
  void            *value;
};

// a table is a collection of Entry* which point to Entries in the pool
// the pool stores the actual data
struct Table {
  size_t  count;
  Entry   **entries;
  Entry   *pool;
};

// Use this before on device, before you start to put
// This will zero out the table
__device__ void zero_out_values_in_table(Table table);
// several header declerations for the newly written functions
__host__ __device__ void iterate(Table table);
__device__ void put(Table table, unsigned int key, Lock *lock, int tid);
__host__ __device__ unsigned long get(Table table, unsigned int key);
// a simple hashing function
__device__ __host__ size_t hash( unsigned int key, size_t count );

// init the table, called from host
void initialize_table( Table &table, int entries, int elements );
// copy the table back to the host, called from host
void copy_table_to_host( const Table &table, Table &hostTable);
// free the tables
void free_table( Table &table );

#endif