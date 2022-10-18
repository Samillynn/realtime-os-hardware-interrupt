#include "k3_user_tasks.h"
#include "../src/name_server.h"
#include "../src/clock_server.h"
#include "../src/syscall.h"
#include "../src/common.h"
#include "../src/printf.h"

STRUCT(K3ClientMsg) {
    u32 delay;
    u32 num;
};

static i32 clock_server_tid;

static void k3_client() {
    K3ClientMsg msg;
    i32 tid = MyTid();
    i32 parent_tid = MyParentTid();

    if (Send(parent_tid, (cstring)&msg, sizeof(K3ClientMsg), (cstring)&msg, sizeof(K3ClientMsg)) < 0) {
        printf("k3_client failed to send\r\n");
        return;
    }

    while (msg.num) {
        printf("task[%d]: (delay: %u, num: %u)\r\n", tid, msg.delay, msg.num);
        Delay(clock_server_tid, msg.delay);
        msg.num -= 1;
    }

    Exit();
}

void k3_first_user_task() {
    Create(5, name_server);
    clock_server_tid = Create(5, clock_server);

    i32 client_idx[64];
    i32 client_priorities[4] = {6, 5, 4, 3};
    u32 client_delays[4] = {10, 23, 33, 71};
    u32 client_nums[4] = {20, 9, 6, 3};

    for (usize i = 0; i < 4; i ++) {
        client_idx[Create(client_priorities[i], k3_client)] = i;
    }

    K3ClientMsg msg;
    i32 tid;
    
    for (usize i = 0; i < 4; i ++) {
        Receive(&tid, (cstring)&msg, sizeof(K3ClientMsg)); {
            msg.delay = client_delays[client_idx[tid]];
            msg.num = client_nums[client_idx[tid]];
        }
        Reply(tid, (cstring)&msg, sizeof(K3ClientMsg));
    }

    Exit();
}