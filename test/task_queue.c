//
// Created by 28379 on 10/5/2022.
//

#include "../src/task.h"

Task _current_task;
Task* current_task = &_current_task;
Task* tasks[100];

int cnt=0;

Task* get_current_task() {
    return current_task;
}

Task* task_queue_add(Task* task) {
    tasks[cnt++] = task;
}

Task* task_queue_get(int tid) {
    for (int i = 0; i < 100; i++) {
        if (tasks[i]->tid == tid) return tasks[i];
    }
    return NULL;
}


