#ifndef __TASK_SCHEDULER_H__
#define __TASK_SCHEDULER_H__

#include "common.h"
#include "config.h"
#include "task.h"
#include "task_scheduler.h"

STRUCT(TaskQueue) {
    Task *front;
    Task *end;
};

extern Task *current_task;

void change_task_state(Task* self, TaskState state);

void scheduler_init();

void task_queue_init();

void task_queue_add(Task *task);

Task* get_task_by_tid(i32 tid);

Task *task_queue_pop();

Task *task_queue_peek();

Task *create_task(i32 priority, void func(), i32 parent_tid);

void remove_current_task();

Task *get_current_task();

Task *schedule();

#endif
