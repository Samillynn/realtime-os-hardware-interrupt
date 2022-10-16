#include "kernel_syscall.h"
#include "task.h"
#include "task_scheduler.h"
#include "printf.h"

/** Create a user task by a parent user task **/
void sys_create() {
  Task *current_task = get_current_task();

  if (current_task != NULL) {
    int priority = current_task->x[0];
    u64 func_ptr = current_task->x[1];
    Task *new_task = create_task(priority, func_ptr, current_task->tid);
    if (new_task != NULL) {
      assign_result(current_task, new_task->tid);
    } else {
      assign_result(current_task, -1);
    }
  } else {
    assign_result(current_task, -1);
  }

}

void sys_tid() {
  Task *current_task = get_current_task();
  if (current_task) {
    assign_result(current_task, current_task->tid);
  } else {
    assign_result(current_task, -1);
  }
}

void sys_parent_tid() {
  Task *current_task = get_current_task();

  if (current_task) {
    assign_result(current_task, current_task->parent_tid);
  } else {
    assign_result(current_task, -1);
  }
}

void sys_yield() {
  // do nothing
}

void sys_exit() {
  remove_current_task();
}


