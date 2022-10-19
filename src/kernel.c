#include "kernel.h"
#include "common.h"
#include "task_scheduler.h"
#include "task.h"
#include "../tasks/name_and_rps_server.h"
#include "config.h"
#include "printf.h"
#include "exception.h"

func_ptr_t initial_user_task;
func_ptr_t idle_task;

void init_tasks() {
    create_task(INITIAL_PRIORITY, initial_user_task, -1);
    create_task(TASK_PRIORITY_MIN, idle_task, -1);
}

void kmain() {
    printf("Start Program\r\n");
    init_tasks();
    while(1) {
        Task *task = schedule();
        if (task != NULL) {
            exception_type_t exception_type = activate(task);
            handle_current_exception(exception_type);
        } else {
            printf("All user tasks have finished, exit elegantly\r\n");
            return;
        }
    }
    printf("Kernel Program Exit\r\n");
}
