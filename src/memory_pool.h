#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__

#include "common.h"

#define MEMORY_POOL_SIZE 0x10000
#define MEMORY_POOL_CHUNK_SIZE 0x80

STRUCT(MemoryPool) {
  u8 data[MEMORY_POOL_SIZE];
  bool used[MEMORY_POOL_SIZE / MEMORY_POOL_CHUNK_SIZE];
};

void memory_pool_init(MemoryPool *self);
void *memory_pool_allocate(MemoryPool *self, usize size);
void memory_pool_free(MemoryPool *self, void *addr);

#endif

