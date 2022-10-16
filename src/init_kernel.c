//
// Created by 28379 on 9/25/2022.
//

#include "init_kernel.h"
#include "kerenel_syscall.h"
#include "rpi.h"
#include "memory.h"
#include "printf.h"
#include "task_scheduler.h"
#include "exception.h"
#include "message_passing.h"

u64 _kernel_regs[31];
u64* kernel_regs = _kernel_regs;

void init_kernel() {
    init_gpio();
    init_spi(0);
    init_uart(0);
    memory_init();
    task_queue_init();
    init_vector_table();
    init_exception_handlers();
}

void init_exception_handlers() {
    exception_handlers[1] = sys_create;
    exception_handlers[2] = sys_yield;
    exception_handlers[3] = sys_exit;
    exception_handlers[11] = sys_tid;
    exception_handlers[12] = sys_parent_tid;
    exception_handlers[21] = sys_send;
    exception_handlers[22] = sys_receive;
    exception_handlers[23] = sys_reply;
}

