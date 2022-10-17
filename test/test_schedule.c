#include "../src/task_scheduler.h"
#include <stdio.h>
#include <assert.h>
#include "../src/debug.h"

void Exit() {}
void T1() {}
void T2() {}
void T3() {}

int main() {
    init_scheduler();
    create_task(1, NULL, -1); // 0

    create_task(1, T1, 0); // 1
    create_task(3, T2, 0); // 2
    create_task(2, T3, 0); // 3

    assert(get_task_by_tid(1)->tid == 1);
    assert(get_task_by_tid(2)->tid == 2);
    assert(get_task_by_tid(3)->tid == 3);

    debug("1: \n");
    change_task_state(schedule(), RUNNING);
    assert(current_task->tid = 2);

    debug("2: \n");
    change_task_state(schedule(), RUNNING);
    assert(current_task->tid = 2);

    Task* t2 = current_task;
    change_task_state(current_task, WAITRECEIVE);

    change_task_state(schedule(), RUNNING);
    assert(current_task->tid == 3);

    debug("3: \n");
    change_task_state(schedule(), RUNNING);
    assert(current_task->tid == 3);

    remove_current_task();

    change_task_state(schedule(), RUNNING);
    assert(current_task->tid == 0);
    remove_current_task();

    change_task_state(schedule(), RUNNING);
    assert(current_task->tid == 1);
    remove_current_task();

    assert(schedule() == NULL);

    change_task_state(t2, READY);
    assert(schedule()->tid == 2);
    change_task_state(t2, RUNNING);
    remove_current_task();
    assert(schedule() == NULL);
}