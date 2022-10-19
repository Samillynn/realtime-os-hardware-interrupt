#include "memory_pool.h"
#include "assert.h"

void memory_pool_init(MemoryPool *self) {
  for (usize i = 0; i < MEMORY_POOL_SIZE; i ++) {
    self->data[i] = 0;
  }
  self->unused_top = 0;
  for (usize i = 0; i < MEMORY_POOL_SIZE / MEMORY_POOL_CHUNK_SIZE; i ++) {
    self->unused[self->unused_top] = &self->data[i * MEMORY_POOL_CHUNK_SIZE];
    self->unused_top += 1;
  }
}

void *memory_pool_allocate(MemoryPool *self, usize size) {
  assert(size <= MEMORY_POOL_CHUNK_SIZE);

  if (self->unused_top > 0) {
    self->unused_top -= 1;
    return self->unused[self->unused_top];
  }

  return NULL;
}

void memory_pool_free(MemoryPool *self, void *addr) {
  self->unused[self->unused_top] = addr;
  self->unused_top += 1;
}
