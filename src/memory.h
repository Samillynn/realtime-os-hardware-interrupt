#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"
#include "memory_block.h"

#define MEMORY_BLOCK_MAX_NUM 64

void memory_init();
MemoryBlock* memory_allocate_block();
void memory_free_block(MemoryBlock* memory_block);
void* memory_get_block_end(MemoryBlock* memory_block);

#endif
