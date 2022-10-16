//
// Created by 28379 on 9/25/2022.
//
#include "exception.h"
#include "printf.h"

#define EXCEPTION_CLASS_OFFSET 26
#define SVC_CLASS 0x15

u64 (*exception_handlers[1 << 16 ])(void);

u64 activate(Task *task) {
  current_task = task;

  if (current_task->should_pass_result) {
    current_task->x[0] = current_task->result;
  }

  return activate_current_task();
}

void handle_exception(u64 esr) {
  u8 exception_class = (esr >> EXCEPTION_CLASS_OFFSET);
  if (exception_class == SVC_CLASS) {
    u16 imm = esr; // cast to the last 16 bits
    exception_handlers[imm]();
  } else {
    printf("Unsupported exception class: %u from task[tid:%d]\r\n", esr, current_task->tid);
    printf("PC=%p, X30=%p, X30[COPY]=%p\r\n", current_task->pc, current_task->x[30], current_task->x30_copy);
  }

  return;
}

void print_error(u64 current_el, u64 elr1, u64 esr1) {
  current_el = (current_el >> 2) & 3;
  printf("CurrentEL is %u\r\n", current_el);
  printf("EL1 - ELR is %x; ESR is %u\r\n", elr1, esr1);
}
