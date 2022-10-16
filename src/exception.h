#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "common.h"
#include "task.h"
#include "task_scheduler.h"
#include "printf.h"

extern u64 (*exception_handlers[1 << 16])(void);

void handle_exception(u64 esr);

u64 activate_current_task();

u64 activate(Task* task);

void print_error(u64 current_el, u64 elr1, u64 esr1);

void handle_error();

#endif

