#include "tool.h"
#include <string.h>
#define new_stack(size,type) _new_stack((size),sizeof(type))
#define spush_p(q,v) stack_push(q,(void*)(v))
#define spush(q,v) {void * _temp_ptr_ = (void*)(long)(v); stack_push(q,&_temp_ptr_);}

#define spop(s) stack_pop(s)
#define stack_empty(s) (s->top<0)
struct stack{
int top;
int element_size;
int size;
void * arr;
}typedef stack;

stack * _new_stack(int size,int element_size);

int stack_push(stack * q,const void * a);

void * stack_pop(stack * q);

void * stack_top(stack * q);

void free_stack(stack * q);