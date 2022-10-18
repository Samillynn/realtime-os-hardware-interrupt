#include "clock_server.h"
#include "name_server.h"
#include "syscall.h"
#include "memory_pool.h"
#include "printf.h"
#include "timer.h"

enum ClockServerAction {
  TICK,
  TIME,
  DELAY,
  DELAY_UNTIL,

  CLOCK_SERVER_ACTION_NUM
};

STRUCT(ClockServerMsg) {
  enum ClockServerAction action;
  u32 ticks;
};

STRUCT(DelayQueueNode) {
  i32 tid;
  u32 delay;

  DelayQueueNode* next;
};

STRUCT(DelayQueue) {
  DelayQueueNode* front;
  DelayQueueNode* end;

  usize size;
};

void delay_queue_init(DelayQueue* self) {
  self->front = NULL;
  self->end = NULL;
  self->size = 0;
}

void delay_queue_add(DelayQueue* self, DelayQueueNode* node) {
  node->next = NULL;
  if (self->end) {
    DelayQueueNode* current_node = self->front;
    DelayQueueNode* previous_node = NULL;
    while (current_node && current_node->delay <= node->delay) {
      previous_node = current_node;
      current_node = current_node->next;
    }
    if (previous_node) {
      previous_node->next = node;
      node->next = current_node;
    } else {
      node->next = self->front;
      self->front = node;
    }
  } else {
    self->end = node;
    self->front = node;
  }
  self->size += 1;
}

DelayQueueNode* delay_queue_pop(DelayQueue* self) {
  DelayQueueNode* popped = NULL;

  if (self->front) {
    popped = self->front;
    if (self->front->next) {
      self->front = self->front->next;
    } else {
      self->front = NULL;
      self->end = NULL;
    }
  }

  self->size -= 1;

  return popped;
}

void clock_server_tick() {
  ClockServerMsg msg; {
    msg.action = TICK;
  }

  i32 tid = MyParentTid();

  while (AwaitEvent(WAIT_TIMER_1) >= 0) {
    if (Send(tid, (cstring)&msg, sizeof(ClockServerMsg), (cstring)&msg, sizeof(ClockServerMsg)) < 0) {
      return;
    }
  }

  Exit();
}

void clock_server() {
  RegisterAs("clock_server");
  printf("clock_server successfully registered in name_server\r\n");

  MemoryPool memory_pool;
  memory_pool_init(&memory_pool);

  DelayQueue delay_queue;
  delay_queue_init(&delay_queue);

  u32 ticks = 0;
  i32 tid;
  ClockServerMsg msg;

  // sys_timer_set_comparator(1, 10000);
  Create(5, clock_server_tick);

  while (Receive(&tid, (cstring)&msg, sizeof(ClockServerMsg))) {
    printf("clock_server received msg[%d, %d] from %d\r\n", msg.action, msg.ticks, tid);
    switch (msg.action) {
    case TICK: {
      ticks += 1;
      Reply(tid, (cstring)&msg, sizeof(ClockServerMsg));

      msg.ticks = ticks;

      DelayQueueNode* current_node = delay_queue.front;
      while (current_node && current_node->delay <= ticks) {
        printf("clock_server: waking up %d\r\n", current_node->tid);
        Reply(current_node->tid, (cstring)&msg, sizeof(ClockServerMsg));
        delay_queue_pop(&delay_queue);
        DelayQueueNode* tmp = current_node;
        current_node = current_node->next;
        memory_pool_free(&memory_pool, tmp);
      }
      break;
    }
    case TIME: {
      printf("TIME\r\n");
      msg.ticks = ticks;
      Reply(tid, (cstring)&msg, sizeof(ClockServerMsg));
      break;
    }
    case DELAY: {
      msg.ticks += ticks;
      break;
    }
    case DELAY_UNTIL: {
      if (msg.ticks <= ticks) {
        printf("ticks mismatch\r\n");
        Reply(tid, (cstring)&msg, sizeof(ClockServerMsg));
      } else {
        DelayQueueNode* new_node = (DelayQueueNode*)memory_pool_allocate(&memory_pool, sizeof(DelayQueueNode)); {
          new_node->tid = tid;
          new_node->delay = msg.ticks;
        }
        delay_queue_add(&delay_queue, new_node);
      }
      break;
    }
    default: {
      break;
    }
    }
  }

  Exit();
}

i32 Time(i32 tid) {
  ClockServerMsg msg; {
    msg.action = TIME;
  }

  if (Send(tid, (cstring)&msg, sizeof(ClockServerMsg), (cstring)&msg, sizeof(ClockServerMsg)) < 0) {
    return -1;
  }

  return msg.ticks;
}

i32 Delay(i32 tid, i32 ticks) {
  ClockServerMsg msg; {
    msg.action = DELAY;
    msg.ticks = ticks;
  }

  if (ticks < 0) {
    return -2;
  }

  if (Send(tid, (cstring)&msg, sizeof(ClockServerMsg), (cstring)&msg, sizeof(ClockServerMsg)) < 0) {
    return -1;
  }

  return msg.ticks;
}

i32 DelayUntil(i32 tid, i32 ticks) {
  ClockServerMsg msg; {
    msg.action = DELAY_UNTIL;
    msg.ticks = ticks;
  }

  if (ticks < 0) {
    return -2;
  }

  if (Send(tid, (cstring)&msg, sizeof(ClockServerMsg), (cstring)&msg, sizeof(ClockServerMsg)) < 0) {
    return -1;
  }

  return msg.ticks;
}
