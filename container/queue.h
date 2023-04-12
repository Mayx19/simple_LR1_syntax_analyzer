#include "tool.h"
#include <string.h>
#define qpush(q,v) {void * _temp_ptr_ = (void *)(long)v; queue_push(q,&_temp_ptr_);}
#define qpush_p(q,v) queue_push(q,(void*)(long)(v))
#define qpop(q) queue_pop(q)

#define queue_empty(q) (q->head==q->tail)
struct queue{
int head;
int tail;
int element_size;
int size;
void * arr;
}typedef queue;

queue * new_queue(int size,int element_size);

int queue_push(queue * q,void * a);

void * queue_pop(queue * q);

void * queue_tail(queue * q);

void free_queue(queue * q);