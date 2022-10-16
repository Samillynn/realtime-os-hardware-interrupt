//
// Created by 28379 on 10/4/2022.
//

#include "queue.h"
#include "common.h"
#include "printf.h"
#include "task.h"

int modular_inc(int x, int mod) {
    return (x + 1) % mod;
}

void queue_init(Queue *queue, u64 *container, int capacity) {
    queue->container = container;
    queue->capacity = capacity;
    queue->head = 0;
    queue->tail = 0;
}

bool queue_is_empty(Queue *queue) {
    return queue->head == queue->tail;
}

bool queue_is_full(Queue *queue) {
    int after_tail = modular_inc(queue->tail, queue->capacity);
    if (queue->head == after_tail) {
        return true;
    }
    return false;
}

void queue_print(Queue *queue) {
  for(int i = queue->head; i < queue->tail; i++) {
    Task* t = (Task*) queue->container[i];
    printf("task[%d]-x0=%d; ", t->tid, t->x[0]);
  }
  printf("\r\n");
}

bool queue_push(Queue *queue, u64 elem) {

    if (queue_is_full(queue)) {
        printf("Queue is full, push_queue fails: (head=%d, tail=%d)\r\n", queue->head, queue->tail);
        return false;
    }

    int new_tail = modular_inc(queue->tail, queue->capacity);
    queue->container[queue->tail] = elem;
    queue->tail = new_tail;

    return true;
}

u64 queue_pop(Queue *queue) {
    if (queue_is_empty(queue)) {
        return 0;
    }

    u64 result = queue->container[queue->head];
    queue->head = modular_inc(queue->head, queue->capacity);
    return result;
}

