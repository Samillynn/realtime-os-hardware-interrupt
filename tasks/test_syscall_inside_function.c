//
// Created by 28379 on 10/6/2022.
//

#include "test_syscall_inside_function.h"
#include "../src/syscall.h"
#include "../src/printf.h"

int test_id() {
  return (10 + MyTid() + MyParentTid()) * (10 + MyTid() - MyParentTid());
}

void test_syscall_inside_function() {
  printf("Start program\r\n");
  printf("test tid&pid: %d\r\n", test_id() * test_id());
  printf("Finishing program\r\n");
}
