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
#include "interrupt.h"
#include "timer.h"

u64 _kernel_regs[31];
u64* kernel_regs = _kernel_regs;

void init_kernel() {
    init_gpio();
    init_spi(0);
    init_uart(0);
    memory_init();
    // task_queue_init();
    init_scheduler();
    init_vector_table();
    init_exception_handlers();
    init_gic();

    sys_clock_init(1, TICK_TIME);
}

void handle_timer_1() {
    sys_clock_notify_tick(1);
}

void init_exception_handlers() {
    synchronous_exception_handlers[1] = sys_create;
    synchronous_exception_handlers[2] = sys_yield;
    synchronous_exception_handlers[3] = sys_exit;
    synchronous_exception_handlers[11] = sys_tid;
    synchronous_exception_handlers[12] = sys_parent_tid;
    synchronous_exception_handlers[21] = sys_send;
    synchronous_exception_handlers[22] = sys_receive;
    synchronous_exception_handlers[23] = sys_reply;
    synchronous_exception_handlers[31] = sys_await_event;

    irq_handlers[WAIT_TIMER_1] = handle_timer_1;
}

