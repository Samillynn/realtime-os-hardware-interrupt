//
// Created by 28379 on 10/6/2022.
//
#include "k1_user_tasks.h"
#include "../src/printf.h"
#include "../src/syscall.h"

void k1_demo_user_task() {
  printf("Tid: %d, ParentTid: %d\r\n\n", MyTid(), MyParentTid());
  Yield();
  printf("Tid: %d, ParentTid: %d\r\n\n", MyTid(), MyParentTid());
  Exit();
}

void k1_first_user_task() {
  int priorities[4] = {INITIAL_PRIORITY - 2, INITIAL_PRIORITY - 1, INITIAL_PRIORITY + 1, INITIAL_PRIORITY + 2};
  for (int i = 0; i < 4; i++) {
    int tid = Create(priorities[i], k1_demo_user_task);
    printf("Created: %d\r\n\n", tid);
  }
  printf("FirstUserTask: exiting\r\n\n");
  Exit();
}
