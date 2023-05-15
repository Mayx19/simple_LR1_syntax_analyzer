#include "syntax.h"
#include "stdio.h"
#define _PROD_R_LEN_DEFAULT 16

_act *new_act(char type, uint id, uint vt, _act *next)
{
    _act *res = _new(_act);
    res->type = type;
    res->id = id;
    res->next = next;
    return res;
}

uint32 max_prod_r_len(_syntax *g)
{
    int mw = 0, t;
    for (int i = 0; i < g->prod_len; i++)
    {
        t = (g->prod[i].r_len + 1);
        if (t > mw)
            mw = t;
    }
    return mw;
}
//

uint32 icompare(void *a, void *b)
{
    _i *ia = (_i *)a;
    _i *ib = (_i *)b;
    if (ia->n != ib->n)
        return 1;
    return memcmp(ia->ptr, ib->ptr, sizeof(int) * ia->n);
}

uint32 ihash(void *a)
{
    _i *ia = (_i *)a;
    uint32 hash = 0;
    for (int i = 0; i < ia->n; i++)
    {
        hash = ia->ptr[i] + (hash << 6) + (hash << 16) - hash;
    }
    stack *s = new_stack(32, int);
    return hash;
}

void next_map(_syntax *g, char *first)
{
}
char *get_null_list(_syntax *g)
{
    printf("nmnmnm\n");
    _init_new_arr(null_list, char, g->vn_len, -1);
    stack *s = new_stack(32, int);
    int i, prod_id, curr, find = 0, prod_len;
    for (i = 0; i < g->vn_len; i++)
        spush(s, i);
    while (!stack_empty(s))
    {
        curr = *((int *)stack_top(s));
        printf("nm %d", curr);
        if (null_list[curr] < 0)
        {
            null_list[curr] = 2;
            for (prod_id = 0; prod_id < prod_len; prod_id++)
            {
                _prod prod = g->prod[prod_id];
                if (g->prod[prod_id].l == curr)
                {
                    if (prod.r_len == 0)
                    {
                        null_list[curr] = 1;
                        prod_id = prod_len;
                        print_prod(g, prod_id, -1);
                        break;
                    }
                    else
                    {
                        for (i = 0; i < prod.r_len; i++)
                        {
                            _v v = prod.r[i];
                            if (v.type == 0)
                            {
                                null_list[curr] = 1;
                                prod_id = prod_len;
                                break;
                            }
                            else
                            {
                                if (null_list[v.id] == 0 || null_list[v.id] == 2)
                                    continue;
                                else if (null_list[v.id] == -1)
                                {
                                    spush(s, curr);
                                    prod_id = prod_len;
                                    break;
                                }
                                else if (null_list[v.id] == 1)
                                {
                                    null_list[curr] = 1;
                                    prod_id = prod_len;
                                    break;
                                }
                            }
                        }
                        if (i == prod.r_len)
                        {
                            null_list[curr] = 1;
                            break;
                        }
                    }
                }
            }
            if (prod_id == prod_len)
            {
                null_list[curr] = 1;
                stack_pop(s);
            }
        }
        else
        {
            stack_pop(s);
        }
    }
    for (i = 0; i < g->vn_len; i++)
    {
        printf("%s %d\n", syntax_t_get_v(g, i, 1), null_list[i]);
    }
    return null_list;
}
void _firstmap_prod(_syntax *g, char *first)
{
    int
        vt_len = g->vt_len,
        prod_len = g->prod_len,
        mx = max_prod_r_len(g);
    int code, prod_id = g->start, pos, suffix, push, top;
    _v v;

    _init_new_arr(flag, char, prod_len *mx, 0);

    stack *s = new_stack(32, int);
    int prl = g->prod[prod_id].r_len;
    first[prod_encode(g, prod_id, prl, 0)] = 1;
    flag[prl * prod_len + prod_id] = 2;
    for (prod_id = 0; prod_id < prod_len; prod_id++)
    {
        prl = g->prod[prod_id].r_len;
        for (pos = 0; pos < prl; pos++)
        {
            int stt = (pos * prod_len + prod_id);
            spush(s, stt);
        }
        flag[prl * prod_len + prod_id] = 2;
    }

#ifdef DEBUG
    char test_prod[100];
    printf("-------------first_prod------------\n");
#endif
    while (!stack_empty(s))
    {
        top = *((int *)stack_pop(s));
        pos = top / prod_len;
        prod_id = top % prod_len;
        push = 0;
        v = g->prod[prod_id].r[pos];
        if (flag[top] != 2)
        {
#ifdef DEBUG
            sprint_prod(test_prod, g, prod_id, pos);
            printf("\nNEXT : %s \n", test_prod);
            printf(" first:");
            for (int i = 0; i < vt_len; i++)
                printf("%d", first[prod_encode(g, prod_id, pos, i)]);

#endif
            flag[top] = 1;
            if (!v.type)
                first[prod_encode(g, prod_id, pos, v.id)] = 1;
            else
            {
                for (int j = 0; j < prod_len && !push; j++)
                {
                    if (g->prod[j].l == v.id && v.type)
                    {
#ifdef DEBUG
                        sprint_prod(test_prod, g, j, 0);
                        printf("\tfind %s  %d ", test_prod, g->prod[j].r_len);
#endif
                        if (g->prod[j].r_len > 0)
                        {
                            if (g->prod[j].r[0].type)
                            {
                                switch (flag[j])
                                {
                                case 0:
                                    spush(s, top);
                                    spush(s, j);
                                    push = 1;
#ifdef DEBUG
                                    sprint_prod(test_prod, g, j, 0);
                                    printf("\n\t[back & push %s] \n", test_prod);
#endif
                                    break;
                                case 2:
                                    for (int i = 0; i < vt_len; i++)
                                        first[prod_encode(g, prod_id, pos, i)] = first[prod_encode(g, prod_id, pos, i)] | first[prod_encode(g, j, 0, i)];
                                    // printf(" result: ");
                                    // for (int i = 0; i < vt_len; i++)
                                    //     printf("%d", first[prod_encode(g, prod_id, pos, i)]);
                                    break;
                                }
                            }
                            else
                            {
                                first[prod_encode(g, prod_id, pos, g->prod[j].r[0].id)] = 1;
                            }
                        }
                        else // if (pos < g->prod[prod_id].r_len)
                        {
                            int stt = (pos + 1) * prod_len + prod_id;
                            switch (flag[stt])
                            {
                            case 0:
                                spush(s, top);
                                spush(s, stt);
                                push = 1;
                                IFDEBUG printf("push %d %d\npush %d %d\n", prod_id, pos, prod_id, pos + 1);
                                break;
                            case 2:
                                for (int i = 0; i <= vt_len; i++)
                                    first[prod_encode(g, prod_id, pos, i)] = first[prod_encode(g, prod_id, pos, i)] | first[prod_encode(g, prod_id, pos + 1, i)];

                                break;
                            }
                        }
                    }
                }
            }

            if (!push)
                flag[top] = 2;
        }
    }
    free(flag);
#ifdef DEBUG
    char test_str[100];
    for (prod_id = 0; prod_id < prod_len; prod_id++)
    {
        int prl = g->prod[prod_id].r_len;
        for (pos = 0; pos <= prl; pos++)
        {
            _setarrall(test_str, 0);
            int testlen = 0;
            int mk = 0;
            for (suffix = 0; suffix < vt_len; suffix++)
            {
                if (first[prod_encode(g, prod_id, pos, suffix)])
                {
                    testlen += sprintf(&test_str[testlen], "%s,", syntax_get_v(g, suffix));
                }
            }
            sprint_prod(test_prod, g, prod_id, pos);
            printf("FIRST( %s ) = {%s}\n", test_prod, test_str);
        }
    }
    printf("-------------------------\n");
#endif
}

#define add_i_temp                                  \
    _closure(g, curr_ptr, &curr_prod_len, null_vn); \
    if (ilen >= ilist_capacity)                     \
        _double_len(_i, ilist);                     \
    ilist[ilen].ptr = curr_ptr;                     \
    ilist[ilen].n = curr_prod_len;                  \
    ilist[ilen].id = ilen;

#define add_i_submit                     \
    curr_ptr = &curr_ptr[curr_prod_len]; \
    qpush(q, ilen);                      \
    hput(imap, &ilist[ilen], ilen);      \
    ilen++;

#define _ILIST_INIT_CAPACITY_ 1024

uint LR1(_syntax *g, _act *action_goto, int cnm)
{
#ifdef PRINT_NODE
    FILE *node_str = fopen("../../demo/nodes.js", "w");
    FILE *edge_str = fopen("../../demo/edges.js", "w");
    if (node_str)
        fprintf(node_str, "nodes= [");

    if (edge_str)
        fprintf(edge_str, "edges= [");
#endif
    g->prod_r_max_len = max_prod_r_len(g);
    int vt_len = g->vt_len;
    int prod_len = (g->prod_len);
    int vl = (g->vn_len + g->vt_len);
    int mx = max((g->prod_len) * g->prod_r_max_len, vl);

    int target;
    // mark null vn
    char *null_vn = cnm ? get_null_list(g) : NULL;

    int first_len = mx * g->prod_len * max(g->prod_len * g->prod_r_max_len, vl) + 1024;
    _init_new_arr(first, char, first_len, 0);

    new_auto_arr(ilist, _i, _ILIST_INIT_CAPACITY_, 0);
    new_auto_arr(map, char, 4096, 0);

    int prod_list[4096];

    _prod *p = g->prod;
    //生成first表
    _firstmap_prod(g, first);

    int *curr_ptr = prod_list;
    int curr_prod_len = 0, ilen = 0;
    int code, prod_id, pos, suffix, priority;

    // hashmap init
    _hashmap *imap = new_hashmap(32);
    imap->compare = icompare;
    imap->element_size = sizeof(void *);
    imap->hash = ihash;

    // queue init
    queue *q = new_queue(1024, sizeof(long));
    // add new I state
    prod_list[curr_prod_len++] = prod_encode(g, g->start, 0, g->prod[g->start].r_len * prod_len + g->start);
    add_i_temp;

    add_i_submit;

    /////////////////
    while (!queue_empty(q))
    {
        //*(ilist[queue_pop(q)]);
        _i *curr_i = &(ilist[*(long *)queue_pop(q)]);
        //遍历每个符号路径
        IFDEBUG printf("find I_%d n:%d:\n", curr_i->id, curr_i->n);
        for (int j = 1; j < vl; j++)
        {
            memset(map, 0, map_capacity);
            curr_prod_len = 0;

            for (int i = 0; i < curr_i->n; i++)
            {
                code = *(curr_i->ptr + i);
                prod_decode(g, code, &prod_id, &pos, &suffix);

                if ((pos < p[prod_id].r_len &&
                     ((p[prod_id].r[pos].type ? (vt_len) : 0) + p[prod_id].r[pos].id) == j)) //||(p[prod_id].r[pos].type&&null_vn[p[prod_id].r[pos]])
                {
                    int new_code = prod_encode(g, prod_id, pos + 1, suffix);
                    // debug codes ,don't delete this
                    /*if ((curr_i->id == 16 && j == 1)||new_code==4284)
                    {
                        printf("!!!BUG \nI_%d --%s--> code[%d]", curr_i->id, syntax_get_v(g, j), new_code);
                        print_prod(g, prod_id, pos);
                        log("\n");
                    }*/
                    _lengthen_to(char, map, new_code);

                    if (!map[new_code])
                    {
                        curr_ptr[curr_prod_len++] = new_code;
                        map[new_code] = 1;
                        priority = prod_id;
                    }
                }
            }
            if (curr_prod_len)
            {
                IFDEBUG printf("\nI_%d --%s--> closure I_%d curr_size:%d:\n ", curr_i->id, syntax_get_v(g, j), ilen, curr_prod_len);
                add_i_temp;
                long ilen_t;
                if (hget(imap, &ilist[ilen], &ilen_t))
                    target = ilist[ilen_t].id;
                else
                {
                    target = ilen;
                    add_i_submit
                        update_action_goto_r(g, &ilist[target], first, action_goto);
                }
#ifdef PRINT_NODE

                if (edge_str)
                    fprintf(edge_str,
                            "\n{\n\"source\": \"I_%d\",\n\"target\": \"I_%d\",\n\"label\":\"%s\",\n},",
                            curr_i->id, target, syntax_get_v(g, j));
#endif
                // write action_goto table "move forward"
                if (action_goto)
                {
                    _act *act_ptr = &action_goto[curr_i->id * vl + j];
                    // ambiguous syntax
                    if (act_ptr->type == _ACTION_GOTO_OP_NULL)
                    {
                        act_ptr->type = _ACTION_GOTO_OP_MOVE_FORWORD;
                        act_ptr->id = target;
                        act_ptr->vt = j;
                    }
                    else
                    {
                        _act *curr = new_act(_ACTION_GOTO_OP_MOVE_FORWORD, target, j, act_ptr->next);
                        if (priority < act_ptr->id)
                        {
                            _swap(_act, *curr, *act_ptr);
                        }
                        else
                        {
                            while (act_ptr->next && act_ptr->next->id < priority)
                                act_ptr = act_ptr->next;
                        }

                        _insert_after(act_ptr, curr);
                    }
                }
            }
        }
    }
    ////////////////////
#ifdef DEBUG
    char test_str[100];

    printf("-----------[STATE LIST]----------\n");
    for (int i = 0; i < ilen; i++)
    {
        _i it = ilist[i];
        printf("I_%d at %d\n", it.id, it.ptr);
        int pre = 0;
        for (int j = 0; j < it.n; j++)
        {
            prod_decode(g, *(it.ptr + j), &prod_id, &pos, &suffix);
            print_prod(g, prod_id, pos);
            sprint_prod(test_str, g, suffix % prod_len, suffix / prod_len);
            printf(";\tFIRST(%s)\n", test_str);
        }
        printf("\n");
    }
    printf("--------------------------\n");
#endif
#ifdef PRINT_NODE
    if (node_str)
    {
        node_json(g, ilist, ilen, first, node_str);
        fprintf(node_str, "\n]");
        fclose(node_str);
    }
    if (edge_str)
        fprintf(edge_str, "\n]");
    fclose(edge_str);
    //  printf("\nvar data={\n\"nodes\": [%s]\n,\"edges\": [%s]\n}", node_str, edge_str);
#endif
    // free memory
    free(first);
    return ilen;
}

void update_action_goto_r(_syntax *g, _i *it, char *first, _act *action_goto)
{
    char suffix_temp[1024];

    int code, prod_id, pos, suffix, pre;
    int vt_len = g->vt_len;
    int mx = max((g->prod_len) * g->prod_r_max_len, (g->vn_len + g->vt_len));
    int vl = (g->vn_len + g->vt_len);
    int j = 0;
    while (j < it->n)
    {
        // find suffix
        _setarrall(suffix_temp, 0);
        do
        {
            prod_decode(g, *(it->ptr + j), &prod_id, &pos, &suffix);

            for (int k = 0; k < vt_len; k++)
                suffix_temp[k] = suffix_temp[k] | first[suffix * mx + k];
            pre = *(it->ptr + j) / mx;
            j++;
        } while (j < it->n && (*(it->ptr + j) / mx) == pre);
        IFDEBUG
        {
            print_prod(g, prod_id, pos);
            // for (int k = 0; k < g->vt_len; k++)
            //     printf("%d", suffix_temp[k]);
            printf(" \t:|");
            if (suffix_temp[0])
                printf("#|");
            for (int k = 1; k < g->vt_len; k++)
                if (suffix_temp[k])
                    printf("%s|", g->vt[k]);
            printf("\n");
        }

        for (int k = 0; k < g->vt_len; k++)
        {
            if (suffix_temp[k])
            {
                // write action_goto table "return" or "accept"
                if (action_goto && g->prod[prod_id].r_len == pos)
                {
                    _act *act_ptr = &action_goto[it->id * +vl + k];
                    // return priorty
                    // if (act_ptr->type == 0 || prod_id < act_ptr->id)
                    // {
                    //     act_ptr->type = (prod_id == g->start ? _ACTION_GOTO_OP_ACCEPT : _ACTION_GOTO_OP_RETURN);
                    //     act_ptr->id = prod_id;
                    // }
                    if (act_ptr->type == _ACTION_GOTO_OP_NULL)
                    {
                        act_ptr->type = (prod_id == g->start ? _ACTION_GOTO_OP_ACCEPT : _ACTION_GOTO_OP_RETURN);
                        act_ptr->id = prod_id;
                        act_ptr->vt = k;
                    }
                    else
                    {
                        _act *curr = new_act((prod_id == g->start ? _ACTION_GOTO_OP_ACCEPT : _ACTION_GOTO_OP_RETURN), prod_id, k, act_ptr->next);
                        if (prod_id < act_ptr->id)
                            _swap(_act, *curr, *act_ptr);
                        while (act_ptr->next && prod_id > act_ptr->next->id)
                            act_ptr = act_ptr->next;
                        _insert_after(act_ptr, curr);
                    }
                }
            }
        }
    }
}

void node_json(_syntax *g, _i *ilist, int ilen, char *first, FILE *node_str)
{
    char suffix_temp[1024];
    char temp_str[1024];

    int code, prod_id, pos, suffix;
    int temp_str_pos, pre, j;
    int vt_len = g->vt_len;
    int mx = max((g->prod_len) * g->prod_r_max_len, (g->vn_len + g->vt_len));

    for (int i = 0; i < ilen; i++)
    {

        _setarrall(suffix_temp, 0);
        _i it = ilist[i];
        pre = 0;
        j = 0;
        temp_str_pos = 0; // printf("I_%d at %d\n", it.id, it.ptr);
        while (j < it.n)
        {
            // find suffix
            do
            {
                prod_decode(g, *(it.ptr + j), &prod_id, &pos, &suffix);

                for (int k = 0; k < vt_len; k++)
                    suffix_temp[k] = suffix_temp[k] | first[suffix * mx + k];

                pre = *(it.ptr + j) / mx;
                j++;
            } while (j < it.n && (*(it.ptr + j) / mx) == pre);
            IFDEBUG
            {
                print_prod(g, prod_id, pos);
                printf(" \t:|");
                if (suffix_temp[0])
                    printf("#|");
                for (int k = 1; k < g->vt_len; k++)
                    if (suffix_temp[k])
                        printf("%s|", g->vt[k]);
                printf("\n");
            }
            // print prod
            temp_str_pos += sprint_prod(&temp_str[temp_str_pos], g, prod_id, pos);
            // print suffix
            temp_str_pos += sprintf(&temp_str[temp_str_pos], ", ");
            for (int k = 0; k < g->vt_len; k++)
            {
                if (suffix_temp[k])
                {
                    temp_str_pos += sprintf(&temp_str[temp_str_pos], "/%s", syntax_get_v(g, k));
                }
            }
            //
            temp_str_pos += sprintf(&temp_str[temp_str_pos], "\\n\\\n");
        }
#ifdef PRINT_NODE

        if (node_str)
            fprintf(node_str,
                    "\n{\n\"id\" : \"I_%d\",\n\"label\":\"I_%d\",\n\"description\":\"%s\",\n\"cluster\": \"a\"\n},\n",
                    it.id, it.id, temp_str);
#endif
    }
}

void prod_t_init(_prod *p, uint l)
{
    p->l = l;
    p->r_capacity = _PROD_R_LEN_DEFAULT;
    p->r = new_arr(_v, _PROD_R_LEN_DEFAULT);
    p->r_len = 0;
}

void prod_append(_prod *p, uint a, char type)
{
    if (p->r_len >= p->r_capacity)
        _double_len(_v, p->r);
    p->r[p->r_len].type = type;
    p->r[p->r_len].id = a;
    p->r_len++;
}

int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int prod_encode(_syntax *g, int prod_id, int pos, int suffix)
{
    // printf("(%d*%d+%d)*%d+%d\n", pos, g->prod_len, prod_id, max(g->prod_len * g->prod_r_max_len, (g->vn_len + g->vt_len)), suffix);
    return (pos * g->prod_len + prod_id) * max(g->prod_len * g->prod_r_max_len, (g->vn_len + g->vt_len)) + suffix;
}

void prod_decode(_syntax *g, int code, int *prod_id, int *pos, int *suffix)
{
    int mx = max((g->prod_len) * g->prod_r_max_len, (g->vn_len + g->vt_len));
    int t = code / mx;
    *suffix = code % mx;
    *prod_id = t % g->prod_len;
    *pos = t / g->prod_len;
    // printf("(%d*%d+%d)*%d+%d\n", *pos, g->prod_len, *prod_id, max(g->prod_len * g->prod_r_max_len, (g->vn_len + g->vt_len)), *suffix);
}
#ifdef DEBUG
#define _closure_add_log                                                \
    printf("    [ADD ");                                                \
    print_prod(g, n_prod_id, n_pos);                                    \
    sprint_prod(test_str, g, n_suffix % prod_len, n_suffix / prod_len); \
    printf("\tsuffix =  FIRST(%s)] code=%d at stack %d\n", test_str, next, n_temp);
#else
#define _closure_add_log
#endif
#define _add_to_closure                                    \
    int next = prod_encode(g, n_prod_id, n_pos, n_suffix); \
    _lengthen_to(char, map, next);                         \
    if (!map[next])                                        \
    {                                                      \
        _closure_add_log;                                  \
        res[n_temp++] = next;                              \
        map[next] = 1;                                     \
    }
// get closure set
void _closure(_syntax *g, int *res, int *closure_size, const char *null_vn)
{
    _prod *p = g->prod;
    int prod_len = (g->prod_len);
    int mx = max((g->prod_len) * g->prod_r_max_len, (g->vn_len + g->vt_len));
    int vt_len = g->vt_len;
    int vn_len = g->vn_len;

    new_auto_arr(map, char, 4096, 0);
    //防止重复
    int n_temp = *closure_size;
#ifdef DEBUG
    char test_str[100];
    printf("CLOSURE{----------------\n");
#endif
    for (int i = 0; i < n_temp; i++)
    {
        auto_arr_set(char, map, res[i], 1);
    }
    // bfs prodections

    for (int i = 0; i < n_temp; i++)
    {
        int prod_id, pos, suffix;
        int n_prod_id, n_pos, n_suffix;
        prod_decode(g, res[i], &prod_id, &pos, &suffix);
#ifdef DEBUG
        printf("at stack %d =%d;", i, res[i]);
        print_prod(g, prod_id, pos);
        sprint_prod(test_str, g, suffix % prod_len, suffix / prod_len);
        printf("\tsuffix[%d]: FIRST(%s)\n", suffix, test_str);
#endif
        //非终结符
        if (pos < p[prod_id].r_len && p[prod_id].r[pos].type)
        {
            _v curr = p[prod_id].r[pos];
            // if (null_vn && null_vn[curr.id])
            // {
            //     n_prod_id = prod_id;
            //     n_pos = pos + 1;
            //     n_suffix = suffix;
            //     _add_to_closure
            // }
            //查生成表
            for (int j = 0; j < prod_len; j++)
            {
                if (p[j].l == curr.id)
                {
                    if (p[j].r_len > 0)
                    {
                        n_prod_id = j;
                        n_pos = 0;
                        if (null_vn)
                        {
                            int suffix_pos = pos;
                            do
                            {
                                n_suffix = (suffix_pos + 1 == p[prod_id].r_len) ? suffix //如果在产生式尾部
                                                                                : (suffix_pos + 1) * prod_len + prod_id;
                                curr = p[prod_id].r[suffix_pos++];
                                _add_to_closure;
                            } while (suffix_pos < p[prod_id].r_len && (curr.type == 1 && null_vn[curr.id]));
                        }
                        else
                        {
                            n_suffix = (pos + 1 == p[prod_id].r_len) ? suffix //如果在产生式尾部
                                                                     : (pos + 1) * prod_len + prod_id;
                            // printf("follow =  FIRST(%s) ", syntax_get_v(g, follow));
                            _add_to_closure
                        }
                    }
                }
            }
        }
    }
    qsort(res, n_temp, sizeof(int), compare);
    *closure_size = n_temp;
    free(map);
#ifdef DEBUG
    printf("------------------------}\n");
#endif
}

uint32 print_prod(_syntax *g, const uint32 p, const uint32 p_i)
{
    uint32 pos = 0;
    _prod prod = g->prod[p];
    pos += printf("%s->", g->vn[prod.l]);
    int i = 0;

    for (; i < prod.r_len; i++)
    {
        if (p_i >= 0 && i == p_i)
            pos += printf("%c ", '\'');
        pos += printf("%s ", syntax_t_get_v(g, prod.r[i].id, prod.r[i].type));
    }
    if (p_i >= 0 && i == p_i)
        pos += printf("%c ", '\'');

    return pos;
}

uint32 sprint_prod(char *tar, _syntax *g, const uint32 p, const uint32 p_i)
{
    uint32 pos = 0;
    _setarrall(tar, 0);
    _prod prod = g->prod[p];
    pos += sprintf(&tar[pos], "%s -> ", g->vn[prod.l]);
    int i = 0;
    for (; i < prod.r_len; i++)
    {
        if (p_i >= 0 && i == p_i)
            pos += sprintf(&tar[pos], "%c ", '\'');
        syntax_t_get_v(g, prod.r[i].id, prod.r[i].type);
        pos += sprintf(&tar[pos], "%s ", syntax_t_get_v(g, prod.r[i].id, prod.r[i].type));
    }
    if (p_i >= 0 && i == p_i)
        pos += sprintf(&tar[pos], "%c ", '\'');

    return pos;
}

char *syntax_get_v(_syntax *g, uint32 i)
{
    if (i == 0)
        return "#";
    if (i < g->vt_len)
        return g->vt[i];

    return g->vn[i - g->vt_len];
}

char *syntax_t_get_v(_syntax *g, uint32 i, char type)
{
    if (type == 0)
        return g->vt[i];
    else
        return g->vn[i];
}

struct _shorten_chain_temp
{
    uint vt, in, out;
    _act *act;
};

uint merge_single_chain(_syntax *syntax, _act *act_go_map, uint i_count, uint **shortcut_str_res)
{

    int row_len = syntax->vn_len + syntax->vt_len;
    _act *iter;
    struct _shorten_chain_temp *chain = new0arr(struct _shorten_chain_temp, i_count);
    uint *vt_c = new0arr(uint, row_len);
    // mark the in-degree and out-degree of each 'I'
    for (int i = i_count - 1; i > 0; i--)
    {
        for (int j = 0; j < row_len; j++)
        {
            iter = &act_go_map[i * row_len + j];
            do
            {
                if (iter->type)
                    vt_c[j]++;

                if (iter->type == _ACTION_GOTO_OP_MOVE_FORWORD)
                {
                    chain[iter->id].in++;
                    chain[i].out++;
                    chain[i].vt = j;
                    chain[i].act = iter;
                }
                iter = iter->next;
            } while (iter);
        }
    }

    new_auto0arr(shortcut_str, uint, 64);
    uint cur_str_len = 0;
    uint pos, act_temp, curr;
    //
    for (int i = 0; i < i_count; i++)
    {
        if (chain[i].out == 1 && chain[i].out != chain[i].in)
        {
            pos = 0;
            curr = chain[curr].act->id;
            if (chain[curr].in == 1 && chain[curr].out == chain[curr].in)
            {
                do
                {
                    auto_arr_set(uint, shortcut_str, cur_str_len + (++pos), chain[curr].vt);
                    chain[i].act->next = chain[curr].act->next;
                    curr = chain[curr].act->id;
                    chain[curr].in = 0;
                } while (chain[curr].in == 1 && chain[curr].out == chain[curr].in);

                shortcut_str[cur_str_len] = pos;
                chain[curr].in = 1;
                chain[curr].act->id = curr;
                chain[curr].act->vt = cur_str_len;

                cur_str_len += pos + 1;
            }
        }
    }
    uint line = 0;
    for (int i = 0; i < i_count; i++)
    {
        if (!chain[i].in)
            i++;
        if (i > line)
            memcpy(act_go_map + row_len * i, act_go_map + row_len * line, sizeof(_act) * row_len);
    }
    *shortcut_str_res = shortcut_str;
    free(chain);
    free(vt_c);
    return line;
}

void print_action_goto_table(_syntax *syntax, _act *act_go_map, uint i_count)
{

    printf("\n\t");
    _act *curr;
    char line[1024];
    uint line_pos = 0;
    uint cnt = 0;
    int row_len = syntax->vn_len + syntax->vt_len;
    _init_new_arr(arr, _act *, row_len, 0);
    for (int j = 0; j < row_len; j++)
    {
        printf("\t%d", j);
    }
    printf("\n\t");
    for (int j = 0; j < row_len; j++)
    {
        printf("\t%s", syntax_get_v(syntax, j));
    }
    printf("\n");
    for (int i = 0; i < i_count && i < syntax->prod_len * max_prod_r_len(syntax); i++)
    {
        line_pos = 0;
        cnt = 0;
        for (int j = 0; j < row_len; j++)
        {
            arr[j] = &act_go_map[i * row_len + j];
            if (arr[j]->type)
                cnt++;
        }
        while (cnt)
        {
            line_pos = 0;
            cnt = 0;
            for (int j = 0; j < row_len; j++)
            {
                curr = arr[j];
                if (curr)
                {
                    switch (curr->type)
                    {
                    case _ACTION_GOTO_OP_ACCEPT:
                        line_pos += sprintf(line + line_pos, "\tacc");
                        break;
                    case _ACTION_GOTO_OP_MOVE_FORWORD:
                        line_pos += sprintf(line + line_pos, "\tS%d", curr->id);
                        break;
                    case _ACTION_GOTO_OP_RETURN:
                        line_pos += sprintf(line + line_pos, "\tr%d", curr->id);
                        break;
                    default:
                        line_pos += sprintf(line + line_pos, " \t");
                        break;
                    }
                }
                else
                {
                    line_pos += sprintf(line + line_pos, " \t");
                }

                line[line_pos] = '\0';
                if (arr[j])
                {
                    arr[j] = arr[j]->next;
                    if (arr[j])
                        cnt++;
                }
            }
            printf("\t%d%s\n", i, line);
        }
    }
}
#define PRARMS_STACK_LOG                          \
    IFDEBUG                                       \
    {                                             \
        printf("\n");                             \
        for (int i = 0; i <= istack->top; i++)    \
            printf("%d ", *(long *)(params + i)); \
        printf("\n");                             \
    }

void syntax_analysis(
    _syntax *syntax,
    _act *act_go_map,
    const char *str,
    int (*next_lexicon)(const char *, int *, void *),
    long *params,
    void *(**func)(void *, int))
{
    uint row_len = syntax->vn_len + syntax->vt_len,
         v_id = 0,
         acc = 0,
         i_state = 0,
         prod_len = 0,
         str_pos = 0,
         str_pos_pre = 0;
    stack *istack = new_stack(64, long);
    stack *input_stack = new_stack(64, long);
    _act curr;
    spush(istack, 0);
    v_id = next_lexicon(str, &str_pos, params + (istack->top));
    PRARMS_STACK_LOG
    while ((!acc) && (v_id >= 0))
    {
        curr = act_go_map[i_state * row_len + v_id];
        IFDEBUG printf("\tnext_lexicon :\"%s\"\n", syntax_get_v(syntax, v_id));
        switch (curr.type)
        {
        case _ACTION_GOTO_OP_MOVE_FORWORD:
            i_state = curr.id;
            IFDEBUG printf("move to I_%d\n", i_state);
            spush(istack, i_state);
            str_pos_pre = str_pos;
            v_id = next_lexicon(str, &str_pos, params + istack->top);
            continue;
        case _ACTION_GOTO_OP_RETURN:
            str_pos = str_pos_pre;
            v_id = syntax->prod[curr.id].l + syntax->vt_len;
            prod_len = syntax->prod[curr.id].r_len;
            PRARMS_STACK_LOG;
            for (int i = 0; i < prod_len; i++)
                spop(istack);
            PRARMS_STACK_LOG;
            IFDEBUG printf("params[%d] = %d %d  ", istack->top,params[istack->top],params[istack->top+2]);
            if (func[curr.id])
                func[curr.id](params, istack->top);
            i_state = *(long *)stack_top(istack);
            IFDEBUG
            {
                printf("current params :%d at %d \n", *(long *)(params + istack->top), istack->top);
                printf("\tprod_%d return to [I_%d] \n\tnext input: [%s]\n", curr.id, i_state, syntax_get_v(syntax, v_id));
            }
            continue;
        case _ACTION_GOTO_OP_ACCEPT:
            acc = 1;
            func[0](params, istack->top);
            IFDEBUG printf("acc \n");
            return;
        default:
            printf(" invalid syntax!\n");
            return;
        }
    }
}
