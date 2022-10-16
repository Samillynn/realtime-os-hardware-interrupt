//
// Created by 28379 on 10/4/2022.
//

#ifndef K2_MESSAGE_PASSING_H
#define K2_MESSAGE_PASSING_H

#include "common.h"
#include "task.h"
void sys_send();
void sys_receive();
void sys_reply();

STRUCT(SendArgs) {
    i64 tid;
    char* msg;
    i64 msg_len;
    char* reply;
    i64 reply_len;
};

STRUCT(ReceiveArgs) {
    int* tid;
    char* msg;
    i64 msg_len;
};

STRUCT(ReplyArgs) {
    i64 tid;
    char *reply;
    i64 reply_len;
};

SendArgs *get_send_args(Task *task);

ReceiveArgs *get_receive_args(Task *task);

ReplyArgs *get_reply_args(Task *task);

#endif //K2_MESSAGE_PASSING_H
