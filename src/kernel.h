#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "common.h"
#include "task.h"

#define PSTATE_IRQ 7
#define PSTATE_FIQ 6

extern func_ptr_t initial_user_task;
extern func_ptr_t idle_task;

void boot();
void kmain();

#endif
