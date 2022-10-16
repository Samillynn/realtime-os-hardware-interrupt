//
// Created by 28379 on 10/4/2022.
//

#ifndef K2_QUEUE_H
#define K2_QUEUE_H
#include "common.h"

STRUCT(Queue) {
        u64 *container;
        int capacity;
        int size;
        int head;
        int tail;
};

void queue_init(Queue *queue, u64 *container, int capacity);
bool queue_is_empty(Queue *queue);
bool queue_push(Queue *queue, u64 elem);
u64 queue_pop(Queue *queue);
void queue_print(Queue *queue);

#endif //K2_QUEUE_H
