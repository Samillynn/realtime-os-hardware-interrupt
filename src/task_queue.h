#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H_

#include "common.h"
#include "task.h"
#include "config.h"

STRUCT(TaskNode) {
    Task* task;
    TaskNode* prev;
    TaskNode* next;
};

void task_node_init(TaskNode* node, Task* task, TaskNode* prev, TaskNode* next);

STRUCT(TaskNodePool) {
    int available_indices[TASK_POOL_SIZE];
    int size;
    TaskNode nodes[TASK_POOL_SIZE];
};

void task_node_pool_init(TaskNodePool*);
TaskNode* task_node_alloc(TaskNodePool*);
void task_node_free(TaskNodePool*, TaskNode*);

STRUCT(TaskList) {
    u32 size;
    TaskNode* head;
    TaskNode* tail;
    TaskNodePool* pool;
};

void task_list_init(TaskList*, TaskNodePool*);
bool task_list_empty(TaskList*);
Task* task_list_remove_node(TaskList* list, TaskNode* node);
Task* task_list_pop_front(TaskList*);
void task_list_push_back(TaskList* list, Task* node);

STRUCT(PriorityTaskQueue) {
    u32 size;
    TaskList task_list[TASK_PRIORITY_MAX];
    TaskNodePool* pool;
};

void ptask_queue_init(PriorityTaskQueue*, TaskNodePool*);
u32 ptask_queue_empty(PriorityTaskQueue*);
Task* ptask_queue_pop(PriorityTaskQueue*);
void ptask_queue_push(PriorityTaskQueue*, Task*);

#endif