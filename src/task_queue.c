#include "task_queue.h"
#include "printf.h"
#include "assert.h"
#include "debug.h"


void task_node_init(TaskNode* node, Task* task, TaskNode* prev, TaskNode* next) {
    node->task = task;
    node->prev = prev;
    node->next = next;
}

void task_node_pool_init(TaskNodePool* pool) {
    assert(pool);

    for(int i=0; i<MAX_TASK_NUM; i++) {
        pool->available_indices[i] = i;
    }
    pool->size = MAX_TASK_NUM;
}

TaskNode* task_node_alloc(TaskNodePool* pool) {
    assert(pool);

    
    // pool->top should be strictly greater than 0 when alloc is called
    assert_p(0 < pool->size && pool->size <= MAX_TASK_NUM, "TaskPool size %d is out of bound\n", pool->size);


    TaskNode* result = pool->nodes + pool->available_indices[--(pool->size)];

    debug("malloc task from pool(%p)n with task(%p)\r\n", pool, result->task);
    task_node_init(result, NULL, NULL, NULL);
    return result;
}

void task_node_free(TaskNodePool* pool, TaskNode* node) {
    assert(pool && node);

    // pool->top should be strictly less than MAX when free is called
    assert(0 <= pool->size && pool->size < MAX_TASK_NUM);

    debug("free task node(%d) from pool(%p)\r\n", node->task->tid, pool);
    int index = node - pool->nodes;
    assert(0 <= index && index < MAX_TASK_NUM);
    pool->available_indices[(pool->size)++] = index;
}


void task_list_init(TaskList* list, TaskNodePool* pool) {
    assert(list && pool);

    list->pool = pool;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

bool task_list_empty(TaskList* list) {
    return list->size == 0;
}

void task_list_push_back(TaskList* list, Task* task) {
    assert(list);

    TaskNode* node = task_node_alloc(list->pool);
    task_node_init(node, task, list->tail, NULL);

    if (list->size) {
        list->tail->next = node;
        list->tail = node;
    } else {
        assert(list->tail == NULL && list->head == NULL);
        list->head = list->tail = node;
    }
    list->size += 1;
}

// Task* task_list_pop_front(TaskList* list) {
//     assert(list->size && list->head && list->tail);

//     TaskNode* node = list->head;

//     Task* task = node->task;
//     TaskNode* next = node->next;

//     if (node->next) {
//         list->head = next;
//         list->head->prev = NULL;
//     } else {
//         list->head = list->tail = NULL;
//     }
//     list->size -= 1;

//     task_node_free(list->pool, node);
//     return task;
// }

Task* task_list_remove_node(TaskList* list, TaskNode* node) {
    assert(list && node);

    if(node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    if(node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
    list->size -= 1;

    Task* task = node->task;
    task_node_free(list->pool, node);
    return task;
}

Task* task_list_pop_front(TaskList* list) {
    task_list_remove_node(list, list->head);
}

void ptask_queue_init(PriorityTaskQueue* pq, TaskNodePool* pool) {
    pq->pool = pool;
    pq->size = 0;
    for(int i=0; i < TASK_PRIORITY_MAX; i++) {
        task_list_init(&(pq->task_list[i]), pq->pool);
    } 
}

u32 ptask_queue_empty(PriorityTaskQueue* pq) {
    return pq->size == 0;
}

Task* ptask_queue_pop(PriorityTaskQueue* pq) {
    assert(pq->size);

    for(int i=TASK_PRIORITY_MAX-1; i>=0; i--) if(!task_list_empty(&(pq->task_list[i]))) {
        return task_list_pop_front(&(pq->task_list[i]));
    }
    pq->size -= 1;

    return NULL; // should never reach this line
}

void ptask_queue_push(PriorityTaskQueue* pq, Task* task) {
    assert(TASK_PRIORITY_MIN <= task->priority && task->priority < TASK_PRIORITY_MAX);

    task_list_push_back(&(pq->task_list[task->priority]), task);
    pq->size += 1;
}
