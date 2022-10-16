#ifndef __MEMORY_BLOCK_H__
#define __MEMORY_BLOCK_H__

#include "common.h"

#define MEMORY_BLOCK_SIZE 0x100000

STRUCT(MemoryBlock) {
  u8 data[MEMORY_BLOCK_SIZE];
};

#endif
