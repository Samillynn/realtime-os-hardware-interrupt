#ifndef __KERNEL_SYSCALL_H__
#define __KERNEL_SYSCALL_H__

#include "common.h"

void sys_create();
void sys_tid();
void sys_parentTid();
void sys_yield();
void sys_exit();
void sys_send();
void sys_receive();
void sys_reply();

#endif
