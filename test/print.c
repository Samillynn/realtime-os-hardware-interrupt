//
// Created by 28379 on 10/5/2022.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "../src/task.h"
#include "../src/message_passing.h"


void printf_(const char *format, ...) {
  va_list arglist;

  va_start(arglist, format);
  vprintf(format, arglist);
  va_end(arglist);
}

void print_task(Task *t) {
  for (int i = 0; i < 8; i++) {
    printf_("x[%d] = %llu\r\n", i, t->x[i]);
  }
  printf_("tid = %d\r\n", t->tid);
}

void print_send(SendArgs *send_args) {
  printf_("    [sys_send]: [send_args] tid=%d, msg=%s, msg_len=%d, reply(%p)=%s, reply_len=%d\r\n",
          send_args->tid, send_args->msg, send_args->msg_len, send_args->reply, send_args->reply,
          send_args->reply_len);
}

void print_receive(ReceiveArgs *args) {
  printf_("    [sys_receive]: length/tid=%d, msg=%s, msg_len=%d\r\n", args->tid, args->msg,
          args->msg_len);
}

void print_reply(ReplyArgs *args) {
  printf_("    [sys_reply]: [args] tid=%d, reply(%p)=%s, reply_len=%d\r\n",
          args->tid, args->reply, args->reply, args->reply_len);
}