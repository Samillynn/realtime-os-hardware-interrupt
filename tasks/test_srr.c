//
// Created by 28379 on 10/5/2022.
//
#include "../src/syscall.h"
#include "../src/printf.h"
#include "../src/timer.h"

int msg_len = 0;
int receiver_id;

void task_a() {
//  printf("Tid of A: %d\r\n", MyTid());
  int tid = 1234;
//  printf("*tid = %p\r\n", &tid);
  char a[512];
  int len_msg = Receive(&tid, a, msg_len);
//  printf("Received from %d: %s with length:%d\r\n", tid, a, len_msg);
  char r[512] = "bye";
  Reply(tid, r, msg_len);
}

void task_b() {
  char msg[512] = "Hello!";
  char reply[512];
  u32 time_a = timer_get_system_time_count();
  int len_reply = Send(receiver_id, msg, msg_len, reply, msg_len);
//  printf("Received from %d: %s with length:%d\r\n", 1, reply, len_reply);
  u32 time_b = timer_get_system_time_count();
  u32 delta_time = time_b - time_a;

  time_a = timer_get_system_time_count();
  time_b = timer_get_system_time_count();
  delta_time = delta_time - (time_b - time_a);

  printf("Send operation with message size %d took %d microsends\r\n", msg_len, timer_count_to_ms(delta_time));

}


void test_srr() {
  printf("start initial user task\r\n");
  i32 tid;

  msg_len = 4;
  receiver_id = 1;
  Create(8, task_a);
  Create(8, task_b);

  receiver_id = 3;
  msg_len = 64;
  Create(8, task_a);
  Create(8, task_b);

  receiver_id = 5;
  msg_len = 256;
  Create(8, task_a);
  Create(8, task_b);
  Exit();
}
