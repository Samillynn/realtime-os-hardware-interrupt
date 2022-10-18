//
// Created by 28379 on 10/6/2022.
//

#include "config.h"
#include "kernel.h"
#include "../tasks/test_srr.h"
#include "../tasks/k1_user_tasks.h"
#include "../tasks/test_syscall_inside_function.h"
#include "../tasks/name_and_rps_server.h"
#include "../tasks/test_gic.h"
#include "../tasks/task_test_send_receive_result.h"
#include "../tasks/idle.h"

void config() {
  idle_task = idle;
  initial_user_task = test_gic;
}
