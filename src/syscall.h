#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "common.h"

i32 Create(i32 priority, void (*function)());
i32 MyTid();
i32 MyParentTid();
void Yield();
void Exit();
i32 Send(int tid, const char *msg, int msglen, char *reply, int replylen);
i32 Receive(int *tid, char *msg, int msglen);
i32 Reply(int tid, void *reply, int replylen);

#endif
