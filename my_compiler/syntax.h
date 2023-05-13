
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "../container/queue.h"
#include "../container/stack.h"
#include "../container/hashmap.h"
#include "../container/linkedlist.h"

#define DEBUG
#define PRINT_NODE
//  DEBUG   PRINT_NODE
#ifdef DEBUG
#define IFDEBUG
#else
#define IFDEBUG if (0)
#endif
#define _ACTION_GOTO_OP_NULL 0
#define _ACTION_GOTO_OP_ACCEPT 1
#define _ACTION_GOTO_OP_MOVE_FORWORD 2
#define _ACTION_GOTO_OP_RETURN 3
#define _ACTION_GOTO_OP_SHORTCUT 4

typedef uint32_t uint;

// struct _vt
// {
//     unsigned char type;
//     union _patten
//     {
//         uint8 type;
//         uint (*compare)(void *);
//         char *ch;
//     } patten;
// } typedef _vt;

struct _act
{
    char type;
    uint vt;
    uint id;
    struct _act *next;
} typedef _act;

struct _v
{
    char type;// 0 :Terminal symbols,1: non-terminal symbols
    uint id;
} typedef _v;

struct _prod
{
    uint l;
    uint r_len;
    _v *r;
    uint r_capacity;

} typedef _prod;

struct _prod_t
{
    uint l;
    uint r_len;
    _v *r;
    uint r_capacity;
} typedef _prod_t;

struct _i
{
    uint *ptr;
    uint id;
    uint n;
} typedef _i;

struct _syntax
{
    char **vt;
    char **vn;
    _prod *prod;
    uint vt_len, vn_len, prod_len, prod_r_max_len;
    uint start;
} typedef _syntax;

_act *new_act(char type, uint id,uint vt, _act *next);

void prod_t_init(_prod *p, uint r);

void prod_append(_prod *p, uint a, char type);

uint32 max_prod_r_len(_syntax *g);

uint32 icompare(void *a, void *b);

uint32 ihash(void *a);

void _firstmap_prod(_syntax *g, char *first);

uint LR1(_syntax *g, _act *action_goto,int cnm);

int compare(const void *a, const void *b);

uint32 max_prod_r_len(_syntax *g);

int prod_encode(_syntax *g, int prod_id, int pos, int suffix);

void prod_decode(_syntax *g, int code, int *prod_id, int *pos, int *suffix);

void _closure(_syntax *g, int *res, int *closure_size, const char *null_vn);

uint32 print_prod(_syntax *g, const uint32 p, const uint32 p_i);

uint32 sprint_prod(char *tar, _syntax *g, const uint32 p, const uint32 p_i);

void node_json(_syntax *g, _i *ilist, int ilen, char *first, FILE *node_str);

char *syntax_get_v(_syntax *g, uint32 i);

char *syntax_t_get_v(_syntax *g, uint32 i, char type);

void update_action_goto_r(_syntax *g, _i *it, char *first, _act *action_goto);

uint merge_single_chain(_syntax *syntax, _act *act_go_map, uint i_count,uint ** shortcut_str_res);

void print_action_goto_table(_syntax *syntax, _act *act_go_map, uint i_count);

void syntax_analysis(
    _syntax *syntax,
    _act *act_go_map,
    const char *str,
    int (*next_lexicon)(const char *, int *, void *),
    long *params,
    void *(**func)(void *, int));
