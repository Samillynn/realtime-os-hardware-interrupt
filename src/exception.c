//
// Created by 28379 on 9/25/2022.
//
#include "exception.h"
#include "printf.h"
#include "kernel.h"
#include "assert.h"
#include "debug.h"

#define EXCEPTION_CLASS_OFFSET 26
#define SVC_CLASS 0x15

func_ptr_t synchronous_exception_handlers[1 << 16];
func_ptr_t irq_handlers[1 << 10];

exception_type_t activate(Task *task) {
  debug("Activating task: %d\r\n", task->tid);
  change_task_state(task, RUNNING);

  if (task->should_pass_result) {
    task->x[0] = task->result;
  }

  debug("SPSR.IRQ = %d\r\n", task->spsr & (1 << PSTATE_IRQ));

  return activate_running_task();
}

void handle_synchronous_exception(u64 esr) {
  u8 exception_class = (esr >> EXCEPTION_CLASS_OFFSET);
  if (exception_class == SVC_CLASS) {
    u16 imm = esr; // cast to the last 16 bits
    synchronous_exception_handlers[imm]();
  } else {
    printf("Unsupported exception class: %u from task[tid:%d]\r\n", esr, current_task->tid);
    debug("PC=%p, X30=%p, X30[COPY]=%p\r\n", current_task->pc, current_task->x[30], current_task->x30_copy);
  }

  return;
}


void handle_irq(u32 intid) {
  func_ptr_t handler = irq_handlers[intid];

  assert_p(handler, "Interrupt(%d) is not implemented\r\n", intid);
  
  debug("Handling interrupt(%d) with handler in %p\r\n", intid, handler);
  handler();
  wake_up_irq_blocked_tasks(intid);
}

void handle_current_exception(exception_type_t exception_type) {
  if (exception_type == SYNCHRONOUS_EXCEPTION) {
    u64 esr = 12345;
    asm volatile("mrs %[esr], esr_el1" : [esr] "=r" (esr));
    handle_synchronous_exception(esr);
  } else if(exception_type == IRQ) {
    u32 intid = ack_interrupt();
    handle_irq(intid);
    clear_interrupt(intid);
    debug("============ Check again intid: %d ===================\r\n\n", ack_interrupt());
  }
}

void print_error(u64 current_el, u64 elr1, u64 esr1) {
  current_el = (current_el >> 2) & 3;
  printf("CurrentEL is %u\r\n", current_el);
  printf("EL1 - ELR is %x; ESR is %u\r\n", elr1, esr1);
}
