#include "../src/interrupt.h"
#include "../src/printf.h"
#include "../src/timer.h"
#include "../src/syscall.h"
#include "../src/assert.h"
#include "../src/debug.h"

void subtask1() {
  printf("Start 1\r\n");
  for(int i=0; i<100;i++) {
    int x=0; for(int j=0; j<=100000; j++) 10+10;
    Yield();
    printf("%d: %d\r\n", MyTid(), i);
  }
}

void subtask2() {
  printf("Start 2\r\n");
  for(int i=0; i<100;i++) {
    int x=0; for(int j=0; j<=100000; j++) 10+10;
    Yield();
    printf("%d: %d\r\n", MyTid(), i);
  }
}

void subtask3() {
  printf("Start 3\r\n");
  for(int i=0; i<100;i++) {
    int x=0; for(int j=0; j<=100000; j++) 10+10;
    Yield();
    printf("%d: %d\r\n", MyTid(), i);
  }
}

void test_gic() {
  printf("Start test_gic\r\n");

  Create(3, subtask1);
  Create(3, subtask2);
  Create(3, subtask3);
  printf("idle created\r\n");

  printf("1: Start wait\r\n");
  sys_timer_set_comparator(1, 3 * TIMER_FREQ);
  AwaitEvent(WAIT_TIMER_1);
  printf("1: End Wait\r\n");

  printf("2: Start wait\r\n");
  sys_timer_set_comparator(1, 2 * TIMER_FREQ);
  AwaitEvent(WAIT_TIMER_1);
  printf("2: End Wait\r\n");
}