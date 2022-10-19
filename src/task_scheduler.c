#include "task_scheduler.h"
#include "memory.h"
#include "utilities.h"
#include "syscall.h"
#include "exception.h"
#include "printf.h"
#include "boot.h"

#include "task_queue.h"
#include "debug.h"
#include "assert.h"

#define SPSR_MASK_ALL (7 << 6)
#define SPSR_EL1h (5 << 0)
#define SPSR_VALUE (SPSR_MASK_ALL | SPSR_EL1h)

static Task task_pool[TASK_POOL_SIZE];
static Task *task_pool_ptr;
static TaskQueue task_queue[TASK_PRIORITY_MAX];

static TaskNodePool jobq_pool;
static TaskNodePool readyq_pool;
static TaskNodePool irqq_pool;

static TaskList jobq;
static PriorityTaskQueue readyq;
static PriorityTaskQueue irqq[NUM_INTERRUPT];

void change_task_state(Task* task, TaskState state) {
  assert(task->state != state);

  debug("change state of task(%d) from %d to %d\r\n", task->tid, task->state, state);
  if(state == RUNNING) {
    current_task = task;
  } else if(state == READY) {
    ptask_queue_push(&readyq, task);
  } else {
    // do nothing
  }

  task->state = state;
}

void init_scheduler() {
  task_queue_init();

  task_node_pool_init(&jobq_pool);
  task_list_init(&jobq, &jobq_pool);

  task_node_pool_init(&readyq_pool);
  ptask_queue_init(&readyq, &readyq_pool);

  task_node_pool_init(&irqq_pool);
  for(int i=0; i<NUM_INTERRUPT; i++) {
    ptask_queue_init(&irqq[i], &irqq_pool);
  }
}

static i32 task_id_cnt;

Task *current_task;

void task_queue_init() {
  current_task = NULL;

  for (usize i = 0; i < TASK_POOL_SIZE; i++) {
    task_init(&task_pool[i]);
    if (i < TASK_POOL_SIZE - 1) {
      task_pool[i].next = &task_pool[i + 1];
    } else {
      task_pool[i].next = NULL;
    }
  }

  for (usize i = TASK_PRIORITY_MIN; i < TASK_PRIORITY_MAX; i++) {
    task_queue[i].front = NULL;
    task_queue[i].end = NULL;
  }

  task_pool_ptr = &task_pool[0];

  task_id_cnt = 0;
}

void task_queue_add(Task *task) {
  i32 priority = task->priority;

  if (task_queue[priority].end) {
    task_queue[priority].end->next = task;
    task_queue[priority].end = task_queue[priority].end->next;
  } else {
    task_queue[priority].end = task;
    task_queue[priority].front = task;
  }

  task->next = NULL;
}

// Task* task_queue_get(i32 tid) {
//   for (i32 i = TASK_PRIORITY_MAX - 1; i >= TASK_PRIORITY_MIN; i--) {
//     Task* cur_task = task_queue[i].front;
//     while (cur_task) {
//       if (cur_task->tid == tid) {
//         return cur_task;
//       }
//       cur_task = cur_task->next;
//     }
//   }

//   return NULL;
// }

Task *task_queue_pop() {
  Task *popped = NULL;

  for (i32 i = TASK_PRIORITY_MAX - 1; i >= TASK_PRIORITY_MIN; i--) {
    if (task_queue[i].front) {
      popped = task_queue[i].front;
      if (task_queue[i].front->next) {
        task_queue[i].front = task_queue[i].front->next;
      } else {
        task_queue[i].front = NULL;
        task_queue[i].end = NULL;
      }
      break;
    }
  }

  return popped;
}

Task *task_queue_peek() {
  Task *task = NULL;

  for (i32 i = TASK_PRIORITY_MAX - 1; i >= TASK_PRIORITY_MIN; i--) {
    if (task_queue[i].front) {
      task = task_queue[i].front;
      break;
    }
  }

  return task;
}

Task *create_task(i32 priority, void (*func)(), i32 parent_tid) {
  priority = i32_clamp(priority, TASK_PRIORITY_MIN, TASK_PRIORITY_MAX - 1);
  Task *task = NULL;

  if (task_pool_ptr) {
    task = task_pool_ptr;
    task_pool_ptr = task->next;
    task_init(task);

    task->tid = task_id_cnt;
    task_id_cnt += 1;
    task->parent_tid = parent_tid;
    task->priority = priority;

    task->memory_block = memory_allocate_block();
    task->sp = (u64)memory_get_block_end(task->memory_block);

    task->pc = (u64)func;
    task->x[30] = (u64)Exit;

    task->spsr = 0;

    task->next = NULL;

    task_list_push_back(&jobq, task);
    task->job_node = (void*)jobq.tail;
    change_task_state(task, READY);
  } else {
    printf("Task creating failed\r\n");
  }

  return task;
}

void remove_current_task() {
  task_list_remove_node(&jobq, (TaskNode*)(current_task->job_node));

  memory_free_block(current_task->memory_block);

  task_init(current_task);
  current_task = NULL;
}

Task *get_current_task() {
  return current_task;
}

// static Task *_blocked_tasks[TASK_POOL_SIZE];
// Task *schedule() {
//   if (current_task) {
//     task_queue_add(current_task);
//   }

//   Queue blocked_tasks;
//   queue_init(&blocked_tasks, (u64*)_blocked_tasks, TASK_POOL_SIZE);
// //  printf("schedule init queue\r\n");

// //  printf("tid(%d) in schedule: x[0]=%d,x[1]=%d\r\n", current_task->tid, current_task->x[0], current_task->x[1]);
//   Task* next_task = task_queue_pop();
//   while (next_task && next_task->state != Ready) {
// //    printf("push tid(%d)\r\n", next_task->tid);
// //    queue_print(&blocked_tasks);
//     queue_push(&blocked_tasks, (u64)next_task);
//     next_task = task_queue_pop();
//   }
//   while (!queue_is_empty(&blocked_tasks)) {
// //    queue_print(&blocked_tasks);
//     task_queue_add((Task*)queue_pop(&blocked_tasks));
//   }
//   return next_task;
// }

Task* schedule() {
  if(current_task) {
    debug("state of current task is %d\n", current_task->state);
    assert(current_task);
    debug("[schedule] current_task->tid=%d, state=%d\r\n", current_task->tid, current_task->state);
    assert(current_task->state == RUNNING || current_task->state < READY);
    if(current_task->state == RUNNING) {
      change_task_state(current_task, READY);
    } else {
      debug("Task(%d) is blocked with state %d\r\n", current_task->tid, current_task->state);
    }
  } else {
    debug("Task is destroyed, thus not adding to job queue\r\n");
  }

  Task* result = NULL;
  if (!ptask_queue_empty(&readyq)) {
    result = ptask_queue_pop(&readyq);
  } else {
    // schedule ide task
  }

  return result;
}

void wake_up_irq_blocked_tasks(int intid) {
  assert(0 <= intid && intid < 1022);

  PriorityTaskQueue* cur_intq = &irqq[intid];
  if(!ptask_queue_empty(cur_intq)) {
    Task* task = ptask_queue_pop(cur_intq);
    debug("wake up task(%p) %d\r\n", task, task->tid);
    change_task_state(task, READY);
  } else {
    // printf("Event queue no.%d is empty\r\n", intid);
  }
}

void add_to_irq_queue(int intid, Task* task) {
  assert(0 <= intid && intid < 1022);

  debug("Adding task %d to irqq %d\r\n", task, intid);
  PriorityTaskQueue* cur_intq = &irqq[intid];
  ptask_queue_push(cur_intq, task);
}

Task* get_task_by_tid(i32 tid) {
  for(TaskNode* node = jobq.head; node != NULL; node = node->next) {
    if (node->task->tid == tid) {
      return node->task;
    }
  }
  return NULL;
}