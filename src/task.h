#ifndef __TASK_H__
#define __TASK_H__

#include "common.h"
#include "memory_block.h"
#include "queue.h"

#define MAX_SENDER_CAPACITY 64

typedef enum {
    INITIALIZED,
    WATISEND, WAITRECEIVE, WATIREPLY, AWATIEVENT,
    READY,
    RUNNING
} TaskState;

STRUCT(Task) {
    u64 x[31];
    u64 sp;
    u64 spsr;
    u64 pc;
    u64 x30_copy;
    u64 result;
    bool should_pass_result;

    i32 tid;
    i32 parent_tid;
    i32 priority;
    TaskState state;

    u64 sender_container[MAX_SENDER_CAPACITY];
    Queue senders;
    void* job_node;

    MemoryBlock *memory_block;
    Task *next;
};

void task_init(Task *self);
void assign_result(Task* task, u64 result);

#endif
