//
// Created by 28379 on 9/25/2022.
//

#ifndef CS452_K1_KSYSTEM_CALLS_H
#define CS452_K1_KSYSTEM_CALLS_H

#include "common.h"
#include "task.h"
#include "syscall.h"

extern Task * current_task;

u64 sys_create();
u64 sys_tid();
u64 sys_parent_tid();
void sys_yield();
void sys_exit();

#endif //CS452_K1_KSYSTEM_CALLS_H
