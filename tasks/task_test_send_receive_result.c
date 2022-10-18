#include "../src/interrupt.h"
#include "../src/printf.h"
#include "../src/timer.h"
#include "../src/syscall.h"
#include "../src/assert.h"
#include "../src/debug.h"
#include "../src/name_server.h"

void sender() {
    RegisterAs("S1");
    
    char msg1[] = "Hi Friend";
    char reply1[100];
    int rlen = Send(WhoIs("R1"), msg1, sizeof msg1, reply1, 3);
    printf("reply from R1: %s (len=%d)\r\n", reply1, rlen);
}

void receiver() {
    RegisterAs("R1");

    char msg1[10];
    int tid;
    int msg_len = Receive(&tid, msg1, 10);
    printf("received from %d: %s (len=%d)\r\n", tid, msg1, msg_len);

    char reply[] = "Bye Another Friend";
    int reply_len = Reply(tid, reply, sizeof reply);
    printf("reply to %d: %s (len=%d)\r\n", tid, reply, reply_len);
}

void test_send_receive_result() {
    Create(9, name_server);
    Create(3, sender);
    Create(3, receiver);
}