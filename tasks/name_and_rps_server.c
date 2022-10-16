//
// Created by 28379 on 9/25/2022.
//

#include "name_and_rps_server.h"
#include "../src/syscall.h"
#include "../src/printf.h"
#include "../src/name_server.h"
#include "../src/rps_server.h"

void name_and_rps_server() {
  printf("start initial user task\r\n");
  i32 tid;
  tid = Create(9, name_server);
  printf("name server created\r\n");
  tid = Create(8, rps_server);
  printf("rps server created\r\n");

  for (usize i = 0; i < 50; i ++) {
    tid = Create(7, rps_client);
    printf("client %lu created\r\n", i);
  }

  Exit();
}

