#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <malloc.h>

typedef signed char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef unsigned int uint;
#define char_null 0
#define int_null 0
#define long_null 0
#define int8_null 0
#define int16_null 0
#define int32_null 0
#define int64_null 0
#define uint8_null 0
#define uint16_null 0
#define uint32_null 0
#define uint64_null 0

#ifdef __WIN32__
#define malloc_size(a) _msize(a)
#define _FILE_STDIN "CON"
#else
#define malloc_size(a) malloc_usable_size(a)
#define _FILE_STDIN "/dev/console"
#endif

//#define DEBUG

#define TRUE 1
#define FALSE 0

#define _new(a) ((a *)malloc(sizeof(a)))
#define _new0(a) ((a *)calloc(1, sizeof(a)))

//#define new_str(a) (charmalloc(s)
#define new_array(a, n) ((a *)malloc(sizeof(a) * (n)))
#define new_arr(a, n) ((a *)malloc(sizeof(a) * (n)))
#define new0arr(a, n) ((a *)calloc(n, sizeof(a)))

#define new_auto_arr(arr_name, type, size, v)               \
    type *arr_name = (type *)malloc(sizeof(type) * (size)); \
    memset((arr_name), (v), sizeof(type) * (size));         \
    uint arr_name##_capacity = size;

#define init_auto_arr(arr_name, type, size, v)        \
    arr_name = (type *)malloc(sizeof(type) * (size)); \
    memset((arr_name), (v), sizeof(type) * (size));   \
    arr_name##_capacity = size;

#define new_auto0arr(arr_name, type, size)               \
    type *arr_name = (type *)calloc(size, sizeof(type)); \
    uint arr_name##_capacity = size;

#define new_auto0prealloc(arr_name, type, size)          \
    type *arr_name = (type *)calloc(size, sizeof(type)); \
    uint arr_name##_capacity = size;                     \
    uint arr_name##_len = 1;

#define prealloc_new(arr_name) arr_nam+([arr_name##_len++)
#define _new_arr(arr_name, type, n, v)                     \
    type *(arr_name) = (type *)malloc(sizeof(type) * (n)); \
    memset((arr_name), (v), sizeof(type) * (n));
#define _init_new_arr(arr_name, type, n, v)                \
    type *(arr_name) = (type *)malloc(sizeof(type) * (n)); \
    memset((arr_name), (v), sizeof(type) * (n));

#define real_len(a) (malloc_size(a) / sizeof(*(a)))
#define _setall(a, b) (memset(a, (b), malloc_size(a)))
#define _setarrall(a, b) (memset(a, (b), sizeof(a)))

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a < b) ? (a) : (b))

#define GC_LIST_MODE_RUNTIME 1
#define GC_LIST_MODE_PREALLOCATE 0

#define GC_LIST_MODE GC_LIST_MODE_RUNTIME

#define log(a) printf(a)
// rol qword ptr [arrayD], 32
#define _swap(type, a, b)   \
    {                       \
        type __swap_temp__; \
        __swap_temp__ = a;  \
        a = b;              \
        b = __swap_temp__;  \
    }

#define _xchg(a, b)           \
    asm volatile("xchg %0,%1" \
                 : "=r"(a)    \
                 : "r"(b), "r"(a))

#define _lengthen_to(type, arr, pos)                                                 \
    if (arr##_capacity < (pos))                                                      \
    {                                                                                \
        uint __old_len__ = arr##_capacity;                                           \
        while (arr##_capacity <= pos)                                                \
            arr##_capacity = arr##_capacity << 1;                                    \
        arr = (type *)realloc(arr, sizeof(type) * (arr##_capacity));                 \
        memset(arr + __old_len__, 0, (arr##_capacity - __old_len__) * sizeof(type)); \
    }

#define _lengthen_to0(type, arr, pos)                                \
    if (arr##_capacity < (pos))                                      \
    {                                                                \
        while (arr##_capacity <= pos)                                \
            arr##_capacity = arr##_capacity << 1;                    \
        arr = (type *)realloc(arr, sizeof(type) * (arr##_capacity)); \
    }

#define _double_len(type, arr)                                       \
    {                                                                \
        arr##_capacity = arr##_capacity << 1;                        \
        arr = (type *)realloc(arr, sizeof(type) * (arr##_capacity)); \
    }

#define auto_arr_set(type, arr_name, pos, value) \
    {                                            \
        uint __t_pos__ = (pos);                  \
        _lengthen_to(char, arr_name, __t_pos__); \
        arr_name[__t_pos__] = (value);           \
    }

#define _is_blank(c) (isblank(c) || (c) == '\n' || (c) == '\r')

#ifndef GC_LIST
#define GC_LIST
struct gc_node
{
    char dirty;
    uint _capacity;
    uint item_size;
    uint first;
    uint len;
    struct gc_node *next;
} typedef gc_node;

#define new_gc_list(name, type, n)                                                 \
    type *name = new0arr(type, n);                                                 \
    {                                                                              \
        gc_node *__gc_node_head_temp__ = (gc_node *)name;                          \
        __gc_node_head_temp__->item_size = sizeof(type);                           \
        __gc_node_head_temp__->first = ((sizeof(type) - 1) / sizeof(gc_node)) + 1; \
        __gc_node_head_temp__->len = __gc_node_head_temp__->first;                 \
        __gc_node_head_temp__->_capacity = n;                                      \
    }

#define gc_list_reset(list, type)                                                                        \
    {                                                                                                    \
        gc_node *__gc_node_head_temp2__ = (gc_node *)list;                                               \
        if (__gc_node_head_temp2__->len > __gc_node_head_temp2__->first)                                 \
        {                                                                                                \
            if (!GC_LIST_MODE)                                                                           \
                memset(                                                                                  \
                    ((type *)list) + __gc_node_head_temp2__->first,                                      \
                    0,                                                                                   \
                    sizeof(type) * (__gc_node_head_temp2__->_capacity - __gc_node_head_temp2__->first)); \
            __gc_node_head_temp2__->dirty = TRUE;                                                        \
        }                                                                                                \
        __gc_node_head_temp2__->len = __gc_node_head_temp2__->first;                                     \
    }

#define gc_list_get(list, type)                                                                  \
    list + ((gc_node *)list)->len;                                                               \
    {                                                                                            \
        gc_node *__gc_node_head_temp__ = (gc_node *)list;                                        \
        __gc_node_head_temp__->len++;                                                            \
        if (GC_LIST_MODE && __gc_node_head_temp__->dirty)                                        \
            list[__gc_node_head_temp__->len] = type##_null;                                      \
        if (__gc_node_head_temp__->len >= __gc_node_head_temp__->_capacity)                      \
        {                                                                                        \
            if (__gc_node_head_temp__->next)                                                     \
            {                                                                                    \
                list = (type *)__gc_node_head_temp__->next;                                      \
                gc_list_reset(list, type);                                                       \
            }                                                                                    \
            else                                                                                 \
            {                                                                                    \
                new_gc_list(__temp_gc_list_name__, type, __gc_node_head_temp__->_capacity << 1); \
                __gc_node_head_temp__->next = (gc_node *)__temp_gc_list_name__;                  \
                list = (type *)__temp_gc_list_name__;                                            \
            }                                                                                    \
        }                                                                                        \
    }

#define free_gc_list(node) free_gc_list_((gc_node *)node);
void free_gc_list_(gc_node *node);

#endif

uint64 get_highest_bit(uint64 a);

char *align_left(char *str, int right);

char **split(char *a, const char *b, uint *len);
