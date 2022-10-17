#include "task_queue.h"
// #include "printf.h"
#include "assert.h"

int printf(const char *format, ...);

void task_node_init(TaskNode* node, Task* task, TaskNode* next) {
    node->task = task;
    node->next = next;
}

void task_node_pool_init(TaskNodePool* pool) {
    assert(pool);

    for(int i=0; i<MAX_TASK_NUM; i++) {
        pool->available_indices[i] = i;
    }
    pool->top = MAX_TASK_NUM;
}

TaskNode* task_node_alloc(TaskNodePool* pool) {
    assert(pool);

    // pool->top should be strictly greater than 0 when alloc is called
    assert(0 < pool->top && pool->top <= MAX_TASK_NUM);

    return pool->all_nodes + pool->available_indices[--(pool->top)];
}

void task_node_free(TaskNodePool* pool, TaskNode* node) {
    assert(pool && node);

    // pool->top should be strictly less than MAX when free is called
    assert(0 <= pool->top && pool->top < MAX_TASK_NUM);

    int index = node - pool->all_nodes;
    assert(0 <= index && index < MAX_TASK_NUM);
    pool->available_indices[(pool->top)++] = index;
}


void task_list_init(TaskList* list, TaskNodePool* pool) {
    printf("list=%p,pool=%p\n", list, pool);
    assert(list && pool);

    list->pool = pool;
    list->head = NULL;
    list->tail = NULL;
}

bool task_list_empty(TaskList* list) {
    return list->head == NULL;
}

void task_list_push_back(TaskList* list, Task* task) {
    assert(list);

    TaskNode* node = task_node_alloc(list->pool);
    task_node_init(node, task, NULL);

    if (list->head) {
        list->tail->next = node;
        list->tail = node;
    } else {
        assert(list->tail == NULL);
        list->head = list->tail = node;
    }
}

Task* task_list_pop_front(TaskList* list) {
    assert(list->head && list->tail);

    TaskNode* node = list->head;

    Task* task = node->task;
    TaskNode* next = node->next;

    if (node->next) {
        list->head = next;
    } else {
        list->head = list->tail = NULL;
    }

    task_node_free(list->pool, node);
    return task;
}

void ptask_queue_init(PriorityTaskQueue* pq, TaskNodePool* pool) {
    pq->pool = pool;
    for(int i=0; i < TASK_PRIORITY_MAX; i++) {
        printf("pool=%p\n", pool);
        task_list_init(&(pq->task_list[i]), pq->pool);
    } 
}

Task* ptask_queue_pop(PriorityTaskQueue* pq) {
    for(int i=TASK_PRIORITY_MAX-1; i>=0; i--) if(!task_list_empty(&(pq->task_list[i]))) {
        return task_list_pop_front(&(pq->task_list[i]));
    }

    assert_p(0, "PriorityTaskQueue is empty");
}

Task* ptask_queue_push(PriorityTaskQueue* pq, Task* task) {
    assert(TASK_PRIORITY_MIN <= task->priority && task->priority < TASK_PRIORITY_MAX);

    task_list_push_back(&(pq->task_list[task->priority]), task);
}
