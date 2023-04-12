#include "../tool.h"
#include "stack.h"

stack *_new_stack(int size, int element_size)
{
  stack *s = _new(stack);
  s->arr = (void *)malloc(size * element_size);
  s->element_size = element_size;
  s->size = size;
  s->top = -1;
  return s;
}
int stack_push(stack *s, const void *a)
{
  if (s->top >= s->size)
    return 0;
  char *arr = (char *)s->arr;
  s->top++;
  memcpy(arr + (s->element_size * s->top), a, s->element_size);
#ifdef DEBUG
  printf("\tpush <-%d stack: ", *(long *)a);
  for (int i = 0; i <= s->top; i++)
    printf("%d,", *(long *)(arr + (s->element_size * i)));
  printf("\n");
#endif
  return 1;
}

void *stack_pop(stack *s)
{
  if (s->top < 0)
    return NULL;
  char *arr = s->arr;
  void *res = arr + (s->element_size * s->top);
  s->top--;

#ifdef DEBUG
  //  printf("\tpop %d at %d\n", *((int *)(arr + (s->element_size * s->top))), s->element_size * s->top);
  printf("\tpop stack: ");
  for (int i = 0; i <= s->top; i++)
    printf("%d,", *(long *)(arr + (s->element_size * i)));
  printf("\n");
#endif
  return res;
}

void *stack_top(stack *s)
{
  if (s->top < 0)
    return NULL;
  char *arr = s->arr;
#ifdef DEBUG

  printf("stack: ");
  for (int i = 0; i <= s->top; i++)
    printf("%d,", *(long *)(arr + (s->element_size * i)));
  printf("\n");
#endif
  return (void *)(arr + (s->element_size * s->top));
}

void free_stack(stack *s)
{
  free(s->arr);
  free(s);
}