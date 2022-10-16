#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "common.h"
#include "task.h"

extern u64* kernel_regs;
extern void (*initial_user_task)(void);
void boot();
void kmain();

#endif
