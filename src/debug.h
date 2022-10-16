//
// Created by 28379 on 10/6/2022.
//

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "common.h"

#define debug30(identifier) { \
  u64 sp=123,x29=0x456,x30=0x789; \
  asm volatile(               \
  "mov %[sp], sp\n\t"         \
  "ldp %[x29], %[x30], [sp]"  \
  :[sp]"=r"(sp), [x29]"=r"(x29), [x30]"=r"(x30) \
  );                          \
  printf("[%s]: sp=%d; x29=%p; x30=%p\r\n", identifier, sp, x29, x30); \
}

void print_x30(u64 x30);

#define _debug(...) do {\
  printf("DEBUG: ");\
  printf(__VA_ARGS__);\
} while (0)

#ifdef DEBUG
  #define debug(...) _debug(__VA_ARGS__)
#else
  #define debug(...)
#endif




#endif //K2_DEBUG_H
