#include "tool.h"

#define _insert_after(a, b)        \
    {                              \
        if (b)                     \
            (b)->next = (a)->next; \
        (a)->next = (b);           \
    }

#define _insert_after_next(a, b, _next) \
    {                                   \
        if ((b))                        \
            *_next(b) = *_next(a);      \
        *_next(a) = (b);                \
    }

#define _iter_begin_n(type, iter, head_ptr, _next) \
    {                                              \
        type *iter = head_ptr;                     \
        while (_next(iter))                        \
        {                                          \
            iter = _next(iter);

#define _iter_begin(type, iter, head_ptr) \
    {                                     \
        type *iter = head_ptr;            \
        while (iter->next)                \
        {                                 \
            iter = iter->next;

#define _iter_end \
    }             \
    }

struct linked_node
{
    void *value;
    struct linked_node *next;
} typedef linked_node;

struct linked_node4
{
    uint32 value;
    struct linked_node *next;
} typedef linked_node4;

struct linked_list
{
    linked_node *head;
    linked_node *tail;
    int *size;
} typedef linked_list;

linked_list new_linked_list(void *t);

void ll_insert(linked_list *list, void *value, int n);

void *ll_get(linked_list *list, int n);

void ll_pushback(linked_list *list, void *value);
