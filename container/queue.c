#include <string.h>
#include "../tool.h"
#include "queue.h"

queue *new_queue(int size, int element_size)
{
    queue *q = _new(queue);
    q->arr = (void *)malloc(size * element_size);
    q->element_size = element_size;
    q->size = size;
    q->head = 0;
    q->tail = 0;
    return q;
}
int queue_push(queue *q, void *a)
{
    int tail_new = (q->tail + 1) % q->size;
    if (tail_new == q->head)
        return 0;

    char *arr = (char *)q->arr;
    // printf("new tail %d %d at %d\n",q->tail,q->head,(q->element_size*q->tail));

    memcpy(arr + (q->element_size * q->tail), a, q->element_size);
    q->tail = tail_new;
    return 1;
}

void *queue_pop(queue *q)
{
    int head_cur = q->head;
    if (head_cur == q->tail)
        return NULL;
    int head_new = (q->head + 1) % q->size;

    q->head = head_new;
    char *arr = q->arr;
    // printf("pop %d at %d\n" ,*((int *)(arr+(q->element_size*head_cur))),q->element_size*head_cur);
    return arr + (q->element_size * head_cur);
}

void *queue_tail(queue *q)
{
    if (q->tail == q->head)
        return NULL;
    char *arr = q->arr;
    return (void *)(arr + (q->element_size * q->tail));
}

void free_queue(queue *q)
{
    free(q->arr);
    free(q);
}