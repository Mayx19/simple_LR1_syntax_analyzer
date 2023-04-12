#include <stdint.h>
#include <stdlib.h>
#include "datrie.h"
#include "../container/hashmap.h"
#define DEBUG
datrie *new_datrie()
{
    datrie *trie = _new(datrie);
    trie->base_capacity = 1024;
    trie->tail_capacity = 1024;
    trie->mark = 1;
    trie->base = new0arr(int, trie->base_capacity);
    trie->check = new_arr(int, trie->base_capacity);
    memset(trie->check, -1, 1024 * sizeof(int));
    trie->check[0] = 0;
    trie->tail = new0arr(char, trie->tail_capacity);
    return trie;
}

void print_trie(datrie *trie, int n)
{
    int i;
    printf("\t");

    for (i = 0; i < n; i++)
        printf("%d\t", i);
    printf("\nbase:\t");

    for (i = 0; i < n; i++)
        printf("%d\t", trie->base[i]);
    printf("\ncheck:\t");

    for (i = 0; i < n; i++)
        printf("%d\t", trie->check[i]);
    printf("\ntail:\t");
    for (i = 0; i < trie->mark; i++)
    {
        if (!trie->tail[i])
            printf("\t");
        else
            printf("%d", trie->tail[i]);
    }
    printf("\n\n");
}

void datrie_add_tail(datrie *trie, int pos, char *str)
{
    if (str[0] == 0)
    {
        trie->base[pos] = 0;
        return;
    }
    int len = strlen(str);
    _lengthen_to0(char, trie->tail, len + 1 + trie->mark);
    strcpy(&trie->tail[trie->mark], str);
    trie->base[pos] = -(trie->mark);
    trie->mark += len + 1;
}

void datrie_add_to_leaf(datrie *trie, int parent, char *c)
{

    if (trie->base[parent] == 0)
    {
        if (c[0] == 0)
            return;
        int i = parent + 1;
        for (; trie->check[i] >= 0 || trie->check[i + c[0]] >= 0; i++)
            ;
        trie->base[parent] = i;
        trie->check[i] = parent;
        trie->base[i] = 0;
        trie->check[i + c[0]] = parent;
        datrie_add_tail(trie, i + c[0], c + 1);
    }
}

int datrie_children_count(datrie *trie, int moved_parent, int *children)
{
    int j, children_count = 0, first = trie->base[moved_parent];
    if (moved_parent == 0)
        return trie->tail_capacity;
    for (j = 0; j <= trie->chr_num; j++)
    {
        if (trie->check[first + j] == moved_parent)
        {
            if (children)
                children[children_count] = j;
            children_count++;
        }
    }
    return children_count;
}

// return the new pos of moved_parent
int datrie_move(datrie *trie, int moved_parent, int *children, int children_count)
{
    int j, k = 0, insert_at, first = trie->base[moved_parent], new_parent, new_pos, old_pos;

    for (insert_at = first; k < children_count;)
    {
        insert_at++;
        for (k = 0; k < children_count; k++)
        {
            new_parent = trie->check[insert_at + children[k]];
            if (new_parent >= 0 && new_parent != moved_parent)
                break;
        }
    }
    int *child_p;

    for (j = children_count - 1; j >= 0; j--)
    {
        new_pos = insert_at + children[j];
        old_pos = first + children[j];
        if (trie->base[old_pos] > 0)
        {
            for (int i = 0; i < trie->chr_num; i++)
            {
                child_p = trie->check + trie->base[old_pos] + i;
                if (*child_p == old_pos)
                    *child_p = new_pos;
            }
        }
        trie->base[insert_at + children[j]] = trie->base[first + children[j]];
        trie->base[first + children[j]] = 0;
        trie->check[insert_at + children[j]] = moved_parent;
        trie->check[first + children[j]] = -1;
    }
    trie->base[moved_parent] = insert_at;

    return insert_at;
}
// return the new pos of to_insert
int datrie_move_insert(
    datrie *trie,
    int moved_parent,
    int *children,
    int children_count,
    int to_insert)
{
    int first = trie->base[moved_parent];
    int j, insert_at, curr,
        k = 0,
        new_child = to_insert - first,
        new_parent;

    for (insert_at = first; k < children_count;)
    {
        insert_at++;
        new_parent = trie->check[insert_at + new_child];
        if (new_parent >= 0 && new_parent != moved_parent)
            continue;
        for (k = 0; k < children_count; k++)
        {
            new_parent = trie->check[insert_at + children[k]];
            if (new_parent >= 0 && new_parent != moved_parent)
                break;
        }
    }

    for (j = children_count - 1; j >= 0; j--)
    {
        int new_pos = insert_at + children[j];
        int old_pos = first + children[j];
        int *child_p;
        if (trie->base[old_pos] > 0)
        {
            for (int i = 0; i < trie->chr_num; i++)
            {
                child_p = trie->check + trie->base[old_pos] + i;
                if (*child_p == old_pos)
                    *child_p = new_pos;
            }
        }
        trie->base[insert_at + children[j]] = trie->base[first + children[j]];
        trie->base[first + children[j]] = 0;
        trie->check[insert_at + children[j]] = moved_parent;
        trie->check[first + children[j]] = -1;
    }
    curr = insert_at + new_child;
    trie->base[curr] = 0;
    trie->check[curr] = moved_parent;
    trie->base[moved_parent] = insert_at;

    return curr;
}

// base[s] + c = t
// check[t] =  s

void datrie_insert(datrie *trie, char *str)
{
#ifdef DEBUG
    char *p = str;
    printf("insert [%d", *(p++));
    while (*p)
        printf(",%d", *(p++));
    printf("]\n");

#endif

    char pre = 0, curr = 0;
    int i, len = strlen(str), chr_num = trie->chr_num;
    for (i = 0; i <= len; i++)
    {
        pre = curr;
        curr = trie->base[curr] + str[i];
        if (trie->check[curr] == pre && trie->base[curr] > 0)
            continue;
        if (trie->check[curr] < 0)
        {
            trie->check[curr] = pre;
            datrie_add_tail(trie, curr, str + i + 1);
            return;
        }
        if (trie->check[curr] != pre)
        {
            // adjust base[pre] or base[check[curr]]
            int moved_parent, inserted, j, k = 0, children_count1 = 0, children_count2 = 0;
            int *children1 = new_arr(int, chr_num << 1);
            int *children2 = children1 + chr_num;
            children_count1 = datrie_children_count(trie, pre, children1);
            children_count2 = datrie_children_count(trie, trie->check[curr], children2);

            if (pre == curr || children_count1 + 1 < children_count2)
                curr = datrie_move_insert(trie, pre, children1, children_count1, curr);
            else
            {
                datrie_move(trie, trie->check[curr], children2, children_count2);
                trie->base[curr] = 0;
                trie->check[curr] = pre;
            }
            if (i < len)
                datrie_add_tail(trie, curr, str + i + 1);
            free(children1);
            return;
        }

        if (trie->base[curr] == 0)
        {
            datrie_add_to_leaf(trie, trie->base[curr], str + i + 1);
            return;
        }
        //if (trie->base[curr] < 0)
        {
            int tail_pos = -trie->base[curr];
            int new_pos, frist;
            if (strcmp(str + i + 1, trie->tail + tail_pos) == 0)
                return;
            while (trie->tail[tail_pos] == str[i + 1])
            {
                new_pos = max(curr, trie->tail[tail_pos] + 1);
                for (;; new_pos++)
                    if (trie->check[new_pos] < 0)
                        break;

                trie->base[curr] = new_pos - trie->tail[tail_pos];
                trie->check[new_pos] = curr;
                curr = new_pos;
                tail_pos++;
                i++;
            }
            // now ,trie->tail[tail_pos] != str[i + 1]
            //print_trie(trie, 20);
            int max_c = max(str[i + 1], trie->tail[tail_pos]);
            frist = curr > max_c ? curr - max_c : 1;
            for (;; frist++)
            {
                if (trie->check[frist + str[i + 1]] < 0 && trie->check[frist + trie->tail[tail_pos]] < 0)
                    break;
            }

            trie->base[curr] = frist;
            trie->check[frist + str[i + 1]] = curr;
           //print_trie(trie, 20);
            if (str[i + 1])
                datrie_add_tail(trie, frist + str[i + 1], str + i + 2);
            else
                trie->base[frist + str[i + 1]] = 0;
            trie->check[frist + trie->tail[tail_pos]] = curr;
            trie->base[frist + trie->tail[tail_pos]] = trie->tail[tail_pos] ? -tail_pos - 1 : 0;

            return;
        }
    }
}

int datrie_find(datrie *trie, char *str)
{
    int i = 0;
    char pre = 0, curr = str[0];
    if (trie->check[curr] != 0)
        return 0;
    while (str[i++] && trie->base[curr] > 0)
    {
        pre = curr;
        curr = trie->base[curr] + str[i];
        if (trie->check[curr] != pre)
            return 0;
#ifdef DEBUG
        printf("-> %d", curr);
#endif
    }
    if (str[i] == 0)
        return trie->base[curr] ? 0 : curr;
#ifdef DEBUG
    printf("at %d cmp %s %s", curr, trie->tail - trie->base[curr], str + i + 1);
#endif
    return (!strcmp(trie->tail - trie->base[curr], str + i + 1)) ? curr : 0;
}

void datrie_trim(datrie *trie)
{
}

// void datrie_insert_str(char *str, _hashmap *map, datrie *trie)
// {
//     int b = 0;
//     for (int i = 0; i < strlen(str); i++)
//     {
//         int id;
//         if (!hget(map, str + i, id))
//         {
//             hput(map, str + i, map->cur_capacity + 1);
//         }
//         trie.check[]
//     }
// }

/*
Maybe used when chr_num is big
int *datrie_kmp_nextval(datrie *trie, int root, int *mark, int max_len)
{
int parent, i = 0, flag = mark[0];

_init_new_arr(next, int, max_len, -1);
next[0] = -1;
next[1] = 0;
for (i = 2; i < max_len; i++)
{
    int temp = next[i - 1];
    do
    {
        if (mark[temp] == flag || mark[i] != flag)
        {
            next[i] = temp + 1;
            break;
        }
        else
        {
            temp = next[temp];
        }
    } while (temp > 0);
}

for (i = 1; i < max_len; i++)
{
    if (mark[next[i]] != flag && mark[i] == flag)
        next[i] = next[next[i]];
}
}


// new0arr(int, trie->base_capacity);
           int min_root = 0;
           int *mark;
           int *[3] root;
           int *[3] count;
           int *[3] max_len;
           if (pre > check[curr])
           {
               mark = new0arr(int, trie->base_capacity - check[curr] + 1);
               min_root = check[curr];
               root[1] = mark;
               root[2] = mark + pre - check[curr];
           }
           else
           {
               mark = new0arr(int, trie->base_capacity - pre + 1);
               min_root = pre;
               root[1] = mark + check[curr] - pre;
               root[2] = mark;
           }
           root[1][0] = 1;
           root[2][0] = 2;
           int parent, max_len = 0, j = min_root;
           // get mark arr
           do
           {
               for (j = min_root; j < trie->base_capacity; j++)
               {
                   parent = trie->check[j] - min_root;
                   if (parent >= 0 && mark[parent])
                   {
                       mark[j] = mark[parent];
                       count[mark[parent]]++;
                       max_len[mark[parent]] = j;
                   }
               }
           } while (j + root < trie->base_capacity);
           int type;
           if (count[1] < count[2])
           {
               root[0] = root[1];
               min_root = check[curr];
               max_len[0] = max_len[1] - min_root + 1;
               type = 1;
           }
           else
           {
               root[0] = root[2];
               min_root = pre;
               max_len[0] = max_len[2] - min_root + 1;
               type = 2;
           }
           int *next = datrie_kmp_nextval(trie, root[0], mark, max_len[0]);

           int move_to = 0;
           move = 0;
           bool not_empty;
           for (int match_i = 0; match_i < max_len[0]; match_i++)
           {
               if (match_i >= 0 && mark[match_i] != type && check[move_to + min_root] > 0)
               {
                   match_i = next[i];
               }
               else
               {
                   move_to++;
               }
           }
       }
*/