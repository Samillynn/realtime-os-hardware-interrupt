//
// Created by 28379 on 10/5/2022.
//
#include "../src/message_passing.h"
#include "../src/task.h"
#include "../src/task_scheduler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "print.h"

Task _sender, _receiver;
Task* sender = &_sender, *receiver = &_receiver;

Task _sender2;
Task* sender2 = &_sender2;

char* MSG = "Hello world";
char* REPLY = "Goodbye world";

void test_init() {
    task_init(sender);
    task_init(receiver);
    task_init(sender2);

    task_queue_add(sender);
    task_queue_add(sender2);

    task_queue_add(receiver);

    sender->tid = 1;
    sender2->tid = 3;
    receiver->tid = 2;
}

void test_send_first(Task* sender) {
    current_task = sender;

    SendArgs* send = get_send_args(sender);
    send->tid = 2;
    send->msg = MSG;
    send->msg_len = strlen(send->msg);
    send->reply = malloc(5);
    send->reply_len = 5;

    sys_send();

    assert(sender->state == WaitSend);
}

void test_receive_after_send(int real_tid) {
    current_task = receiver;
    ReceiveArgs * rec = get_receive_args(receiver);
    int* tid = malloc(8);
    rec->tid = tid;
    rec->msg = malloc(100);
    rec->msg_len = 100;

    sys_receive();

    assert(*tid == real_tid);
    assert(rec->msg != MSG);
    printf("rec->msg: %s\r\n", rec->msg);
    assert(strcmp(rec->msg, MSG) == 0);
    assert(receiver->x[0] == strlen(MSG));

    assert(receiver->state == Ready);
    assert(task_queue_get(real_tid)->state == WaitReply);
}

void test_reply() {
    current_task = receiver;
    ReplyArgs *r = get_reply_args(receiver);
    SendArgs *s = get_send_args(sender);
    r->tid = 1;
    r->reply = REPLY;
    r->reply_len = strlen(REPLY);

    sys_reply();

    assert(receiver->state == Ready);
    assert(sender->state == Ready);

    assert(s->reply != REPLY);
    assert(strcmp(s->reply, REPLY) != 0);
    assert(sender->x[0] == 5);
}

void test_receive_first() {
    current_task = receiver;
    ReceiveArgs * rec = get_receive_args(receiver);

    int* tid = malloc(8);
    rec->tid = tid;
    rec->msg = malloc(100);
    rec->msg_len = 100;

    print_receive(rec);
    sys_receive();
    print_receive(rec);

    assert(receiver->state == WaitReceive);
}

//SendArgs* init_send_args(Task* sender) {
//    SendArgs* send = get_send_args(sender);
//    *send = (SendArgs) {
//            .tid = 2,
//            .msg = MSG,
//            .msg_len = strlen(send->msg),
//            .reply = malloc(5),
//            .reply_len = 5
//    };
//}

void test_send_after_receive() {
    current_task = sender;

    SendArgs* send = get_send_args(sender);
    *send = (SendArgs) {
            .tid = 2,
            .msg = MSG,
            .msg_len = strlen(send->msg),
            .reply = malloc(5),
            .reply_len = 5
    };

    ReceiveArgs* rec = get_receive_args(receiver);
    int* rec_tid = receiver->x[0];
    sys_send();

    assert(*rec_tid == 1);
    assert(rec->msg != MSG);
    assert(strcmp(rec->msg, MSG) == 0);
    assert(receiver->x[0] == strlen(MSG));

    assert(receiver->state == Ready);
    assert(sender->state == WaitReply);
}


void test_multiple_senders() {
    test_send_first(sender);
    test_send_first(sender2);
    test_send_first(sender2);
    test_receive_after_send(1);
    test_receive_after_send(3);
    test_receive_after_send(3);
}

int main() {
    test_init();
    test_receive_first();
//    test_init();
//    test_send_first(sender);
//    test_receive_after_send(1);
//    test_reply();
//
//
//    test_send_first(sender);
//    test_receive_after_send(1);
//    test_reply();
//
//    test_receive_first();
//    test_send_after_receive();
//    test_reply();
//
//    test_multiple_senders();
}