#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__

#include "common.h"
#include "task.h"

STRUCT(TaskQueue) {
    Task *front;
    Task *end;
};

extern Task *current_task;

void task_queue_init();

void task_queue_add(Task *task);

Task* task_queue_get(i32 tid);

Task *task_queue_pop();

Task *task_queue_peek();

Task *create_task(i32 priority, void func(), i32 parent_tid);

void remove_current_task();

Task *get_current_task();

Task *schedule();

#endif
