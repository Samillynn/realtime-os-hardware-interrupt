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

void wait(int code, int second) {
  printf("%d: Start wait\r\n", code);
  u64 start = sys_timer_full_count();
  AwaitEvent(WAIT_TIMER_1);
  u64 end = sys_timer_full_count();
  printf("%d: End Wait, time used: %u\r\n", code, end - start - sys_timer_msrmt_error());
}

void test_gic() {
  u64 start = 0, end = 0;
  printf("Start test_gic\r\n");

  // Create(3, subtask1);
  // Create(3, subtask2);
  // Create(3, subtask3);
  // printf("idle created\r\n");

  wait(1, 2);

  wait(2, 3);

  wait(3, 5);

}