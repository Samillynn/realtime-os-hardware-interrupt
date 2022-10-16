#ifndef __INIT_H__
#define __INIT_H__
#include "exception.h"

extern u64* kernel_regs;
void init_kernel();
void init_vector_table();
void init_exception_handlers();

#endif