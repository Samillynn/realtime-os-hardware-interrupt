#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "common.h"
#include "task.h"
#include "task_scheduler.h"
#include "printf.h"

typedef enum {
    SYNCHRONOUS_EXCEPTION = 0,
    IRQ = 1
} exception_type_t;

extern func_ptr_t synchronous_exception_handlers[1 << 16];
extern func_ptr_t irq_handlers[1 << 10];

void handle_current_exception(exception_type_t exception_type);

exception_type_t activate_running_task();

exception_type_t activate(Task* task);

void print_error(u64 current_el, u64 elr1, u64 esr1);

void handle_error();

u32 ack_interrupt();

void clear_interrupt(u32 intid);

#endif

