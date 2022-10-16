#include "memory.h"
#include "printf.h"

static MemoryBlock memory_blocks[MEMORY_BLOCK_MAX_NUM + 1];
static bool memory_blocks_used[MEMORY_BLOCK_MAX_NUM];

void memory_init() {
  for (usize i = 0; i < MEMORY_BLOCK_MAX_NUM; i ++) {
    for (usize j = 0; j < MEMORY_BLOCK_SIZE; j ++) {
      memory_blocks[i].data[j] = 0;
    }
    memory_blocks_used[i] = false;
  }
}

MemoryBlock* memory_allocate_block() {
  MemoryBlock* memory_block = NULL;
  
  for (usize i = 0; i < MEMORY_BLOCK_MAX_NUM; i ++) {
    if (memory_blocks_used[i] == false) {
      memory_block = &memory_blocks[i];
      memory_blocks_used[i] = true;
      break;
    }
  }

  return memory_block;
}

void memory_free_block(MemoryBlock* memory_block) {
  usize idx = memory_block - &memory_blocks[0];
  for (usize i = 0; i < MEMORY_BLOCK_SIZE; i ++) {
    memory_blocks[idx].data[i] = 0;
  }
  memory_blocks_used[idx] = false;
}

void* memory_get_block_end(MemoryBlock* memory_block) {
  return (void*)(&memory_blocks[memory_block - &memory_blocks[0] + 1]);
}
