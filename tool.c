#include "tool.h"

uint64 get_highest_bit(uint64 a)
{
    uint64 res = 0;
    while (a)
    {
        res++;
        a = a >> 1;
    }
    return res;
}

char *align_left(char *str, int right)
{
    if (right <= 0)
        right = strlen(str);

    int start = 0, i = 0, j = right - 1;

    while (_is_blank(str[j]) && j >= 0)
        j--;
    while (_is_blank(str[j]) && i < j)
        i++;
    if (i)
    {
        while (i <= j)
            str[start++] = str[i++];
        memset(&str[start], 0, right - start);
    }
    else
        memset(&str[j + 1], 0, right - j);
    //  printf(" align %s %d",str,str);

    return str;
}

void free_gc_list_(gc_node *node)
{
    gc_node *__temp_gc_node__;
    gc_node *__temp_gc_iter__ = node;
    do
    {
        __temp_gc_node__ = __temp_gc_iter__;
        __temp_gc_iter__ = __temp_gc_iter__->next;
        free(__temp_gc_node__);
    } while (__temp_gc_iter__);
}

char **split(char *a, const char *b, uint *len)
{

    int blen = strlen(b), n = 0;
    int alen = strlen(a);

    char **arr = new_arr(char *, alen);

    char *copy = new_arr(char, alen + 1);
    strcpy(copy, a);
    char *i = strstr(copy, b);
    if (copy != i)
        arr[n++] = copy;
    memset(i, 0, blen);
    i += blen;
    arr[n++] = i;

    while (i = strstr(i, b))
    {
        memset(i, 0, blen);
        i += blen;
        arr[n++] = i;
    }
    char **res = new_arr(char *, n);
    memcpy(res, arr, sizeof(char *) * n);
    for (int i = 0; i < n; i++)
        *len = n;

    free(arr);
    return res;
}