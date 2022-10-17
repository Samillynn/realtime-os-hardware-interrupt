#include "../src/task_queue.h"
#include "../src/task.h"
#include <stdio.h>
#include <assert.h>

void test_pool() {
    TaskNodePool pool;
    task_node_pool_init(&pool);

    TaskNode* n63 = task_node_alloc(&pool);
    printf("No.%ld Node\n", n63 - pool.all_nodes);

    TaskNode* n62 = task_node_alloc(&pool);
    printf("No.%ld Node\n", n62 - pool.all_nodes);

    task_node_free(&pool, n62);

    n62 = task_node_alloc(&pool);
    printf("No.%ld Node\n", n62 - pool.all_nodes);

    task_node_free(&pool, n62);
    task_node_free(&pool, n63);

    TaskNode* n;
    for(int i=0; i<64; i++) n = task_node_alloc(&pool);
    printf("All nodes allocated\n");

    printf("No.%ld Node\n", n - pool.all_nodes);
    task_node_free(&pool, n);
    n = task_node_alloc(&pool);
    printf("No.%ld Node\n", n - pool.all_nodes);

    TaskNode extra;
    task_node_free(&pool, &extra);
}

int test_list() {
    TaskNodePool pool;
    task_node_pool_init(&pool);

    TaskList list;
    task_list_init(&list, &pool);

    Task t1; t1.tid = 1;
    task_list_push_back(&list, &t1);
    Task t2; t1.tid = 2;
    task_list_push_back(&list, &t2);
    Task t3; t1.tid = 3;
    task_list_push_back(&list, &t3);

    assert(task_list_pop_front(&list) == &t1);
    assert(task_list_pop_front(&list) == &t2);
    assert(task_list_pop_front(&list) == &t3);
    assert(task_list_empty(&list));
    // task_list_pop_front(&list);

    Task tasks[100];
    for(int i=0; i<64; i++) task_list_push_back(&list, tasks + i);
    task_list_push_back(&list, tasks);
    for(int i=0; i<64; i++) 
        assert(task_list_pop_front(&list) == tasks + i);
    assert(task_list_empty(&list));


}

void test_ptask_list() {
    TaskNodePool pool;
    task_node_pool_init(&pool);

    PriorityTaskQueue pq;
    ptask_queue_init(&pq, &pool);

    Task t1; t1.priority = 1;
    ptask_queue_push(&pq, &t1);
    Task t2; t2.priority = 3;
    ptask_queue_push(&pq, &t2);

    Task t3; t3.priority = 3;
    ptask_queue_push(&pq, &t3);
    
    Task* s = ptask_queue_pop(&pq);
    assert(s == &t2);
    s = ptask_queue_pop(&pq);
    assert(s == &t3);
    s = ptask_queue_pop(&pq);
    assert(s == &t1);
    s = ptask_queue_pop(&pq);

}

int main() {
    test_ptask_list();
    // test_list();
}