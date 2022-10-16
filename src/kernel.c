#include "kernel.h"
#include "common.h"
#include "task_scheduler.h"
#include "task.h"
#include "../tasks/name_and_rps_server.h"
#include "printf.h"
#include "exception.h"

void (*initial_user_task)(void);

void init_first_user_task() {
    create_task(INITIAL_PRIORITY, initial_user_task, -1);
}

void kmain() {
    printf("Start Program\r\n");
    init_first_user_task();
    while(1) {
        Task *task = schedule();
        if (task != NULL) {
            int request = activate(task);
            handle_exception(request);
        } else {
            printf("All user tasks have finished, exit elegantly\r\n");
            return;
        }
    }
    printf("Kernel Program Exit\r\n");
}
