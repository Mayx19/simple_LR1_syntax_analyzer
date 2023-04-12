//(,),*,+,?,[,],i
#include "syntax.h"
#define PROD_W_START 9
#define PROD_W_RETURN_VT 8
#define PROD_W_ADD_PROD 6
#define PROD_W_MORE 2
#define PROD_W_CHAR 1
#define PROD_W_NULL 0

const char *vt = "\0()*+?[],i";

_hashmap *vt_map;
char str_temp[64];
_syntax *syn;
_prod prod_arr[128];
uint interval[256][3];
uint vt_type[256];
linked_node *nullable_head;
uint prod_id = 0, interval_size = 0;

char prod_name[1024];
int64 res_map[1024];
int64 prod_weight[1024];
char vn_name[1024];
char vt_name[1024];

int prod_name_len, vn_name_len, vt_name_len, res_cnt;

// (|)|*|+|?|[|]|,|i
// Z|P|E|C
// Z->P
// E->(E)
// E->E*
// E->E+
// E->E?
// C->[i,i]
// C->i
// E->EC
// E->EE
// E->C
// P->E
// P->P,E

uint32 print_prod_t(_syntax *g, _prod *prod, const uint32 p_i)
{
    uint32 pos = 0;
    pos += printf("%s-> ", g->vn[prod->l]);
    for (int i = 0; i < prod->r_len; i++)
        pos += printf("%s ", syntax_t_get_v(g, prod->r[i].id, prod->r[i].type));

    return pos;
}

void prod_append_log(_prod *p, uint a, char type)
{
    IFDEBUG
    {
        printf("part r len:%d[", p->r_len);
        print_prod_t(syn, p, -1);
    }
    prod_append(p, a, type);
    IFDEBUG printf("]<<%s\n", syntax_t_get_v(syn, a, type));
}

uint32 _strcmp(void *a, void *b)
{
    return strcmp((char *)a, (char *)b);
}

#define regist_func                                           \
    {                                                         \
        int _func_pos = 0;                                    \
        func[_func_pos++] = acc;               /* Z->P */     \
        func[_func_pos++] = parentheses;       /* E->(E) */   \
        func[_func_pos++] = mutiple_or_none;   /* E->E* */    \
        func[_func_pos++] = more;              /* E->E+ */    \
        func[_func_pos++] = one_or_none;       /* E->E? */    \
        func[_func_pos++] = char_interval;     /* C->[i,i] */ \
        func[_func_pos++] = general_char;      /* C->i */     \
        func[_func_pos++] = append_char;       /* E->EC */    \
        func[_func_pos++] = append_expression; /* E->EE */    \
        func[_func_pos++] = new_prod;          /* E->C */     \
        func[_func_pos++] = first;             /* P->E */     \
        func[_func_pos++] = next;              /* P->P,E */   \
    }

int next_lexicon(const char *str, int *pos, void *params)
{

    int len = 0;
    if (str[*pos] == '\n')
        return 0;
    while (_is_blank(str[*pos]))
        *pos += 1;

    if (str[*pos] == '\\')
    {
        switch (str[*pos + 1])
        {
        case 'b':
            *(long *)params = '\b';
        case 'n':
            *(long *)params = '\n';
            break;
        case 't':
            *(long *)params = '\t';
            break;
        case 'r':
            *(long *)params = '\r';
            break;
        default:
            *(long *)params = str[*pos + 1];
            break;
        }
        (*pos) += 2;
        return 9;
    }
    for (int i = 0; i < 9; i++)
    {
        if (str[*pos] == vt[i])
        {
            (*pos) += 1;
            return i;
        }
    }
    *(long *)params = str[*pos];
    (*pos) += 1;
    return 9;
}

_prod *add_prod_t(uint l)
{
    _prod *np = &prod_arr[syn->prod_len++];
    prod_t_init(np, l);
    return np;
}

uint64 add_vn(_syntax *s)
{
    uint64 res = s->vn_len;
    printf("\tnew vn: E%d ", res);
    s->vn[s->vn_len++] = vn_name + vn_name_len;
    vn_name_len += sprintf(vn_name + vn_name_len, "E%d", res);
    *(vn_name + (vn_name_len++)) = '\0';
    return res;
}

// void *get_type(void *params, int pos)
// {
//     ((uint64 *)params)[pos] = res_map[((uint64 *)params)[pos]];
//     printf("get_type pos:%d =%d", pos, ((uint64 *)params)[pos]);
//     return NULL;
// }

//[P->E]
void *_return_vt(void *params, int pos)
{
    printf("\t[P->E] -->");

    _prod **params_ptr = (_prod **)params;
    // E
    _prod *E = params_ptr[pos];
    // Z->Pn
    printf("\tres_map[%d] = %d\n", E->l, res_cnt);
    res_map[E->l] = res_cnt++;

    prod_weight[syn->prod_len] = PROD_W_RETURN_VT;
    _prod *E_new = add_prod_t(1);
    prod_append_log(E_new, E->l, 1);
    //(uint64 *)params[0] = E->l;
    return NULL;
}

// P->E
void *first(void *params, int pos)
{
    printf("\t[P->E] -->");

    _prod **params_ptr = (_prod **)params;
    // E
    _prod *E = params_ptr[pos];
    // Z->Pn
    printf("\tres_map[%d] = %d\n", E->l, res_cnt);
    res_map[E->l] = res_cnt++;

    prod_weight[syn->prod_len] = PROD_W_ADD_PROD;
    _prod *E_new = add_prod_t(1);
    prod_append_log(E_new, E->l, 1);
}
// P->P,E
void *next(void *params, int pos)
{
    printf("\t[P->P,E] -->");
    _prod **params_ptr = (_prod **)params;
    // E
    _prod *E = params_ptr[pos + 2];
    printf("\tres_map[%d] = %d\n", E->l, res_cnt);
    // Z->Pn
    res_map[E->l] = res_cnt++;

    prod_weight[syn->prod_len] = PROD_W_ADD_PROD;
    _prod *E_new = add_prod_t(1);
    prod_append_log(E_new, E->l, 1);
    // (uint64 *)params[0] = E->l;
    return NULL;
}

void *acc(void *params, int pos)
{
    for (int i = syn->prod_len; i > 1; i--)
    {
        for (int j = 1; j < i; j++)
        {
            if (prod_weight[j] > prod_weight[j - 1])
            {
                _swap(_prod, prod_arr[j], prod_arr[j - 1]);
                _swap(int64, prod_weight[j], prod_weight[j - 1]);
            }
        }
    }
    return NULL;
}

// E->E+

void *more(void *params, int pos)
{
    printf("\t[E->E+] -->");

    _prod **params_ptr = (_prod **)params;
    // E
    _prod *E = params_ptr[pos];
    // E'->E
    uint curr_vn = add_vn(syn);
    prod_weight[syn->prod_len] = PROD_W_RETURN_VT;
    _prod *E_new = add_prod_t(curr_vn);
    prod_append_log(E_new, E->l, 1);
    // E'->EE
    prod_weight[syn->prod_len] = PROD_W_MORE;
    _prod *E_more = add_prod_t(E->l);
    prod_append_log(E_more, E->l, 1);
    prod_append_log(E_more, E->l, 1);

    params_ptr[pos] = E_more;
    return NULL;
}

// E->E*
void *mutiple_or_none(void *params, int pos)
{
    printf("\t[E->E*] -->");

    _prod **params_ptr = (_prod **)params;
    // E
    _prod *E = params_ptr[pos];
    // E->
    prod_weight[syn->prod_len] = PROD_W_NULL;
    _prod *E0 = add_prod_t(E->l);

    linked_node *v_null = _new(linked_node);
    v_null->value = (void *)(uint64)E->l;
    _insert_after(nullable_head, v_null);
    // E'->E
    prod_weight[syn->prod_len] = PROD_W_RETURN_VT;
    uint curr_vn = add_vn(syn);
    _prod *E1 = add_prod_t(curr_vn);
    prod_append_log(E1, E->l, 1);
    // E->EE
    prod_weight[syn->prod_len] = PROD_W_MORE;
    _prod *E2 = add_prod_t(E->l);
    prod_append_log(E2, E->l, 1);
    prod_append_log(E2, E->l, 1);

    params_ptr[pos] = E1;
    return NULL;
}
// E->E?
void *one_or_none(void *params, int pos)
{
    printf("\t[E->E?] -->");

    _prod **params_ptr = (_prod **)params;
    // E
    _prod *E = params_ptr[pos];
    // E->
    prod_weight[syn->prod_len] = PROD_W_NULL;
    _prod *E0 = add_prod_t(E->l);
    linked_node *v_null = _new(linked_node);
    v_null->value = (void *)(uint64)E->l;
    _insert_after(nullable_head, v_null);
    // E'->E
    prod_weight[syn->prod_len] = PROD_W_ADD_PROD;
    int curr_vn = add_vn(syn);
    _prod *E1 = add_prod_t(curr_vn);
    prod_append_log(E1, E->l, 1);

    params_ptr[pos] = E1;
    return NULL;
}
// E->(E)
void *parentheses(void *params, int pos)
{
    printf("\t[E->(E)] -->");
    _prod **params_ptr = (_prod **)params;
    prod_weight[syn->prod_len] = PROD_W_ADD_PROD;
    _prod *E1 = add_prod_t(add_vn(syn));
    _prod *next = params_ptr[pos + 1];
    prod_append_log(E1, next->l, 1);
    params_ptr[pos] = E1;
    return NULL;
}

uint64 add_vt(_syntax *s, const char *str)
{

    uint64 res;
    if (hget(vt_map, str, &res))
        return res;
    char *temp = vt_name + vt_name_len;
    vt_name_len += sprintf(vt_name + vt_name_len, "%s", str);
    vt_name[vt_name_len++] = '\0';
    // log("vt_name:\n");for(int i=0;i<vt_name_len;i++)printf("%d:%c ",i,vt_name [i] );

    res = s->vt_len++;
    s->vt[res] = temp;
    hput(vt_map, temp, res);

    // printf("[i = %s id=%d]", temp, res);
    return res;
}

uint64 get_c(uint64 *param)
{
    uint64 firsts = param[0];
    uint64 curr_vt;

    *(str_temp) = firsts;
    *(str_temp + 1) = '\0';

    if (hget(vt_map, str_temp, &curr_vt))
        return curr_vt;
    *(str_temp) = firsts;
    *(str_temp + 1) = '\0';
    printf("\tnew vt:%s\n", str_temp);
    curr_vt = add_vt(syn, str_temp);
}
// E->EC
void *append_char(void *params, int pos)
{
    printf("\t[E->EC]");
    _prod **params_ptr = (_prod **)params;
    _prod *E = params_ptr[pos];
    uint64 c = *(uint64 *)(params_ptr + pos + 1);
    prod_append_log(E, c, 0);
    return NULL;
}

// E->EE
void *append_expression(void *params, int pos)
{
    printf("\t[E->EE]");

    _prod **params_ptr = (_prod **)params;
    _prod *E = params_ptr[pos];
    _prod *next = params_ptr[pos + 1];
    prod_append_log(E, next->l, 1);
    return NULL;
}

// E->C
void *new_prod(void *params, int pos)
{
    printf("\t[E->C]");
    _prod **params_ptr = (_prod **)params;

    uint64 *param = (uint64 *)params;
    uint64 curr_vt = *(param + pos);
    prod_weight[syn->prod_len] = PROD_W_CHAR;
    _prod *E1 = add_prod_t(add_vn(syn));
    prod_append_log(E1, curr_vt, 0);
    params_ptr[pos] = E1;
    return NULL;
}
// C->i
void *general_char(void *params, int pos)
{
    printf("\t[C->i]");
    uint64 *params_ptr = (uint64 *)params;

    params_ptr[pos] = get_c(params_ptr + pos);
    return NULL;
}
// C->[i-i]
void *char_interval(void *params, int pos)
{
    printf("\t[C->[i,i]]");
    long *params_ptr = (long *)params;
    int curr_vt;

    uint64 a = params_ptr[pos + 1];
    uint64 b = params_ptr[pos + 3];
    uint64 s = a + b;
    a = min(a, b);
    b = s - a;
    sprintf(str_temp, "[%c-%c]", a, b);
    str_temp[6] = '\0';

    if (hget(vt_map, str_temp, &curr_vt))
    {
        params_ptr[pos] = curr_vt;
        return NULL;
    }
    curr_vt = add_vt(syn, str_temp);
    vt_type[curr_vt] = 1;
    interval[interval_size][0] = (uint)a;
    interval[interval_size][1] = (uint)b;
    interval[interval_size][2] = curr_vt;
    interval_size++;
    params_ptr[pos] = curr_vt;

    for (int i = 0; i < interval_size; i++)
    {
        printf(" match interval [%d -%d]->vt%d\n", interval[i][0], interval[i][1], interval[i][2]);
    }
    return NULL;
}

void _init_()
{
    prod_name_len = 0;
    vn_name_len = 0;
    vt_name_len = 0;
    res_cnt = 0;

    // print_prod_t
    _setarrall(prod_name, 0);
    _setarrall(res_map, -1);
    _setarrall(vn_name, 0);
    _setarrall(vt_name, 0);

    vt_map = new_hashmap(64);
    vt_map->compare = _strcmp;
    vt_map->hash = str_hash;
    nullable_head = _new(linked_node);
    nullable_head->next = 0;
    syn = _new(_syntax);
    syn->vn = new_arr(char *, 512);
    syn->vt = new_arr(char *, 512);
    syn->vn_len = 0;
    syn->vt_len = 0;
    syn->start = 0;
    const char *str_null = "";

    add_vt(syn, str_null);

    syn->prod = prod_arr;
    syn->prod_len = 0;
    syn->vn[syn->vn_len++] = vn_name + vn_name_len;
    vn_name_len += sprintf(vn_name + vn_name_len, "Z");
    *(vn_name + (vn_name_len++)) = '\0';
    prod_weight[syn->prod_len] = PROD_W_START;
    _prod *E_new = add_prod_t(0);
    prod_append_log(E_new, 1, 1);
    add_vn(syn);
}

struct _match_state
{
    struct _match_state *parent;
    uint children_cnt;
    int str_pos;
    uint i_state;
    uint depth;
    _act next_act;
} typedef _match_state;

const _match_state _match_state_null = {
    .children_cnt = 0,
    .depth = 0,
    .i_state = 0,
    .parent = 0,
    .str_pos = 0,
    .next_act.id = 0,
    .next_act.next = 0,
    .next_act.type = 0};

void _printf_state_node(_match_state *state_t, char *str_temp)
{
    if (state_t->parent == state_t)
        printf("\nbug\n");

    _setarrall(str_temp, 0);
    _act *curr = &state_t->next_act;
    if (curr->type)
    {
        switch (curr->type)
        {
        case _ACTION_GOTO_OP_ACCEPT:
            sprintf(str_temp, "act:[acc]");
            break;
        case _ACTION_GOTO_OP_MOVE_FORWORD:
            sprintf(str_temp, "act:[S%d]", curr->id);
            break;
        case _ACTION_GOTO_OP_RETURN:
            sprintf(str_temp, "act:[r%d]", curr->id);
            break;
        }
    }
    //  printf("{[I-%d] pos:%d %s parent:%d addr:%d}", state_t->i_state, state_t->str_pos, str_temp, state_t->parent, state_t);
    printf("{[I-%d]at%d %s}", state_t->i_state, state_t->str_pos, str_temp);
}

#define _move_forward                               \
    state_t = gc_list_get(state_pre, _match_state); \
    state_t->parent = parent;                       \
    state_t->depth = parent->depth + 1;             \
    state_t->str_pos = str_pos;                     \
    if (parent)                                     \
        parent->children_cnt++;

#define _add_next_state                                                       \
    _act *iter = act_go_map + (row_len * curr_state->parent->i_state + find); \
    while (iter && iter->type)                                                \
    {                                                                         \
        if (!cnt)                                                             \
        {                                                                     \
            curr_state->next_act = *iter;                                     \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            _move_forward;                                                    \
            state_t->next_act = *iter;                                        \
            state_t->next_act.next = 0;                                       \
            IFDEBUG                                                           \
            {                                                                 \
                printf("[PUSH] %d", state_t);                                 \
                _printf_state_node(state_t, str_temp);                        \
                printf("\n");                                                 \
            }                                                                 \
            spush(input_stack, state_t);                                      \
        }                                                                     \
        cnt++;                                                                \
        iter = iter->next;                                                    \
    }

int loop_check(_match_state *flag, _match_state *curr)
{
    /* printf("\n<I-%d depth:%d pos%d>\n", curr->i_state, curr->depth, curr->str_pos);
     printf("<I-%d depth:%d pos%d> next %d\n", (flag + curr->str_pos + 1)->i_state, (flag + curr->str_pos + 1)->depth, (flag + curr->str_pos + 1)->str_pos, (flag + curr->str_pos + 1)->next_act);*/
    _match_state *temp = (_match_state *)(flag + curr->str_pos + 1)->next_act.next;

    while (temp && (temp->next_act.id != curr->i_state || temp->depth > curr->depth))
        temp = (_match_state *)temp->next_act.next; //  printf("<I-%d depth:%d > ", temp->i_state, temp->depth);

    return temp != 0; // printf("\nloop_check=%d\n", temp != 0);
}

#define find_vt                                                                         \
    {                                                                                   \
        str_pos = curr_state->str_pos;                                                  \
        parent = curr_state->parent;                                                    \
        state_t = curr_state;                                                           \
        char curr[2] = {str[curr_state->str_pos], 0};                                   \
        uint64 find = 0, cnt = 0;                                                       \
        _act *iter;                                                                     \
        if (temp_date->first_type[(state_t->i_state >> 1) + 1])                         \
            for (int i = 0; i < interval_size; i++)                                     \
            {                                                                           \
                if ((uint)curr[0] <= interval[i][1] && (uint)curr[0] >= interval[i][0]) \
                {                                                                       \
                    find = interval[i][2];                                              \
                    _add_next_state;                                                    \
                }                                                                       \
            }                                                                           \
        if (temp_date->first_type[(state_t->i_state >> 1) + 0])                         \
            if (hget(vt_map, curr, &find))                                              \
            {                                                                           \
                _add_next_state;                                                        \
            }                                                                           \
        cnt++; /*following code is for possible empty vn input*/                        \
               /*if cnt>0  states were found will be added to the queue*/               \
        str_pos = curr_state->str_pos - 1;                                              \
        if (temp_date->first_type[(state_t->i_state >> 1) + 3])                         \
            _iter_begin(linked_node, null_v, nullable_head);                            \
        {                                                                               \
            find = (int64)null_v->value + syn->vt_len;                                  \
            _add_next_state;                                                            \
        }                                                                               \
        _iter_end;                                                                      \
    }

#define DEBUG_PRINT_STATE_STACK                                    \
    IFDEBUG                                                        \
    {                                                              \
        state_t = curr_state;                                      \
        do                                                         \
        {                                                          \
            _printf_state_node(state_t, str_temp);                 \
            if (state_t->parent == state_t)                        \
                break;                                             \
            state_t = state_t->parent;                             \
            if (state_t)                                           \
                printf(" -> ");                                    \
        } while (state_t);                                         \
        printf("\n\tPOS[ %d ] = %c\n\t\t", str_pos, str[str_pos]); \
    }
#define __lexical_test_free__ \
    temp_date->flag_capacity = flag_capacity;

#define __pop_match_state_stack__                         \
    if (!stack_empty(input_stack))                        \
    {                                                     \
        IFDEBUG printf("[POP] stack!\n");                 \
        curr_state = *(_match_state **)spop(input_stack); \
        break;                                            \
    }                                                     \
    else                                                  \
    {                                                     \
        IFDEBUG printf(" invalid syntax!\n");             \
        __lexical_test_free__;                            \
        return -1;                                        \
    }

struct lexical_temp
{

    _match_state *flag;
    uint flag_capacity;
    _match_state *state_pre;
    stack *input_stack;
    char *first_type;
} typedef lexical_temp;

lexical_temp *init_lexical_temp(_syntax syntax, _act *act_go_map, uint rows)
{
    lexical_temp *res = _new(lexical_temp);
    uint _rows = rows ? rows : syntax->prod_len * max_prod_r_len(syntax);
    uint row_len = syntax->vn_len + syntax->vt_len;

    new_gc_list(state_pre, _match_state, 128);
    res->state_pre = state_pre;
    res->input_stack = new_stack(128, _match_state);
    res->flag = new0arr(_match_state, 128);
    res->first_type = new0arr(char, _rows << 2);
    for (int i = 0; i < _rows; i++)
    {
        for (int j = 0; j < row_len; j++)
        {
            if (act_go_map[i * row_len + j].type)
            {
                if (j <= syntax.vt_len)
                {
                    if (vt_type[j])
                        res->first_type[(i << 2) + vt_type[j]] = 1;
                    else
                        res->first_type[(i << 2) + 0] = 1;
                }
                else
                {
                    res->first_type[(i << 2) + 3] = 1;
                }
            }
        }
    }
    return res;
}

int lexical_match_first(
    _syntax *syntax,
    _act *act_go_map,
    lexical_temp *temp_date,
    const char *str_start,
    int *pos)
{
#ifdef DEBUG
    char str_temp[64];
#endif
    const char *str = str_start + *pos;

    uint row_len = syntax->vn_len + syntax->vt_len, prod_len = 0, find, i_state = 0;
    int str_pos = 0, reslen = 0, flag_max = -1, result = -1, cnt = 0;

    _match_state *state_t, *parent, *curr_state;

    _match_state *state_pre = temp_date->state_pre;
    _match_state *flag = temp_date->flag;
    uint flag_capacity = temp_date->flag_capacity;
    stack *input_stack = temp_date->input_stack;

    gc_list_reset(state_pre, _match_state);
    input_stack->top = -1;
    parent = gc_list_get(state_pre, _match_state);

    _move_forward;
    curr_state = state_t;

    find_vt;

    while (1)
    {
        //    IFDEBUG printf("\tnext_lexicon :\"%s\"\n", syntax_get_v(syntax, state[curr_input.state_top + 1][1]));
        switch (curr_state->next_act.type)
        {
        case _ACTION_GOTO_OP_RETURN:
            IFDEBUG
            {
                printf("\n\t[RETURN] ");
                print_prod(syn, curr_state->next_act.id, -1);
                printf("\n");
            }
            prod_len = syntax->prod[curr_state->next_act.id].r_len;
            cnt = syntax->prod[curr_state->next_act.id].l;
            find = cnt + syntax->vt_len;
            str_pos = curr_state->str_pos - 1;
            curr_state = curr_state->parent;
            if (res_map[cnt] >= 0)
            {
                result = res_map[cnt];
                *pos += curr_state->str_pos;
            }

            for (int i = 0; i < prod_len; i++)
                curr_state = curr_state->parent;

            parent = curr_state;
            IFDEBUG printf("\tnext input:[%s]\n", syntax_get_v(syn, find));
            _move_forward;
            state_t->next_act.next = 0;
            curr_state = state_t;
            cnt = 0;
            _add_next_state;

            DEBUG_PRINT_STATE_STACK;

            continue;
        case _ACTION_GOTO_OP_MOVE_FORWORD:
            IFDEBUG printf("\n\t[MOVE]  ");

            curr_state->i_state = curr_state->next_act.id;

            str_pos = curr_state->str_pos + 1;
            if (str_pos <= flag_max)
            {
                if (loop_check(flag, curr_state))
                {
                    __pop_match_state_stack__;
                }
                (curr_state)->next_act.next = (_act *)(flag + str_pos)->next_act.next;
            }
            else
                flag_max = str_pos;

            //------insert to top-------
            (flag + str_pos)->next_act.next = (_act *)(curr_state);
            //--------------------------
            parent = curr_state;
            _move_forward;
            curr_state = state_t;
            DEBUG_PRINT_STATE_STACK;
            find_vt;
            continue;
        case _ACTION_GOTO_OP_ACCEPT:
            IFDEBUG printf("acc \n");
            *pos += curr_state->str_pos;
            __lexical_test_free__;
            return result;
        default:
            __pop_match_state_stack__;
            __lexical_test_free__;
            continue;
        }
    }
    return result;
}