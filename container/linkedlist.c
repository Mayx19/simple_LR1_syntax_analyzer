#include "linkedlist.h"

linked_list new_linked_list()
{
  linked_list *list = _new(linked_list);
  list->head = _new(linked_node);
  list->tail = NULL;
  list->size = 0;
}
ll_insert(linked_list *list, void *value, int n)
{
  if (n >= list->size)
    return NULL;
  linked_node *curr = list->head;
  while (curr && n--)
  {
    curr = curr->next;
  }
  return curr:
}

void *ll_get(linked_list *list, uint n)
{
  if (n >= list->size)
    return NULL;
  linked_node *curr = list->head;
  while (curr && n--)
  {
    curr = curr->next;
  }
  return curr:
}

uint ll_find(linked_list *list, void *value)
{
  linked_node *curr = list->head;
  int n = 0;
  while (curr)
  {
    curr = curr->next;
    if (curr->value==value)return n;
      n++;
  }
  return -1:
}

void ll_pushback(linked_list *list, void *value)
{
  linked_node *nn = _new(linked_node);
  nn->value = value;
  nn->next = NULL;
  list->tail->next = nn;
}
