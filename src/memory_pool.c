#include "memory_pool.h"

void memory_pool_init(MemoryPool *self) {
  for (usize i = 0; i < MEMORY_POOL_SIZE; i ++) {
    self->data[i] = 0;
  }
  for (usize i = 0; i < MEMORY_POOL_SIZE / MEMORY_POOL_CHUNK_SIZE; i ++) {
    self->used[i] = false;
  }
}

void *memory_pool_allocate(MemoryPool *self, usize size) {
  if (size <= MEMORY_POOL_CHUNK_SIZE) {
    for (usize i = 0; i < MEMORY_POOL_SIZE / MEMORY_POOL_CHUNK_SIZE; i ++) {
      if (!self->used[i]) {
        self->used[i] = true;
        return (void*)&self->data[i * MEMORY_POOL_CHUNK_SIZE];
      }
    }
  }

  return NULL;
}

void memory_pool_free(MemoryPool *self, void *addr) {
  self->used[((u8*)addr - &self->data[0]) / MEMORY_POOL_CHUNK_SIZE] = false;
}
