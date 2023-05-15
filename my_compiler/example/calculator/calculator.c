#define DEBUG
#include "syntax.h"
#include "calculator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

_hashmap *vmap;
int vn = 1;

char **simple_v(char **str_arr, int len, char type)
{

    char **res = new_arr(char *, len + 1);
    for (int i = 1 - type; i <= len - type; i++)
    {
        res[i] = align_left(str_arr[i - 1 + type], 0);
        _v v = {type, i};
        long *vl = (long *)&v;

        hput(vmap, res[i], *vl);
    }
    vn = len;
    return res;
}

uint32 char_hash(void *key)
{
    return ((char *)key)[0] * 33;
}

uint32 char_cmp(void *a, void *b)
{
    return *(char *)a != *(char *)b;
}

int main(int argc, const char **argv[])
{
    FILE *fp;
    _v v;
    char line[128];
    _syntax *syntax = _new(_syntax);
    int result, prod_arr_size = 0, pos = 0;
    vmap = new_hashmap(64);
    vmap->compare = char_cmp;
    vmap->hash = char_hash;
    // printf("read  %s\n\n",argv[0]);
    if (fp = fopen("./a.txt", "r"))
    { //(char*)argv[i],

        // read vt
        fgets(line, sizeof(line), fp);
        int split_len;
        char **simp = split(line, ",", &split_len);
        syntax->vt = simple_v(simp, split_len, 0);
        syntax->vt_len = split_len+1;
        // read vn
        fgets(line, sizeof(line), fp);
        simp = split(line, ",", &split_len);
        syntax->vn = simple_v(simp, split_len, 1);
        syntax->vn_len = split_len;

        _prod *prod_arr = new_array(_prod, 32);
        _setall(prod_arr, 0);
        prod_arr_size = 0;
        while (fgets(line, sizeof(line), fp) != NULL)
        {
// init prod
//--------------------------r part
#ifdef DEBUG
            printf("%s", line);
#endif
            char *ri = strstr(line, "->");
            if (!ri)
                continue;
            memset(ri, 0, 2); // clean String'->'

            int len, start = 0;
            align_left(line, ri - line);
            hget(vmap, line, &v);
            prod_arr[prod_arr_size].l = v.id;
            ri += 2;
            len = strlen(align_left(ri, 0));

            _init_new_arr(r, _v, len, 0);

            pos = 0;
            while (ri[pos])
            {
                hget(vmap, &ri[pos], &r[pos]);
                pos++;
            }
            prod_arr[prod_arr_size].r = r;
            prod_arr[prod_arr_size].r_len = pos;
            prod_arr_size++;
        }

        fclose(fp);

        syntax->prod = prod_arr;
        syntax->prod_len = prod_arr_size;
        syntax->start = 0;
#ifdef DEBUG
        // check part-------------

        printf("\ncheck\n\n");
        printf("{ \\0 [t0]  ");
        int temp = syntax->vt_len;
        for (int i = 1; i < temp; i++)
        {
            hget(vmap, syntax->vt[i], &v);
            printf(", %s [%c%d] ", (char *)syntax->vt[i], v.type ? 'n' : 't', v.id);
        }
        printf("}\n");
        hget(vmap, syntax->vn[0], &v);
        printf("{ %s [%c%d] ", syntax->vn[0], v.type ? 'n' : 't', v.id);
        for (int i = 1; i < syntax->vn_len; i++)
        {
            hget(vmap, syntax->vt[i], &v);
            printf(", %s [%c%d] ", syntax->vn[i], v.type ? 'n' : 't', v.id);
        }
        printf("}\n\n");
        for (int i = 0; i < syntax->prod_len; i++)
        {
            _prod p = syntax->prod[i];
            printf("%d -> ", p.l);
            for (int j = 0; j < p.r_len; j++)
                printf("%c%d", p.r[j].type ? 'n' : 't', p.r[j].id);
            printf("\n");
        }
#endif

        // NFA_generate(syntax);
        _act act_go_map[2024];

        uint rows = LR1(syntax, act_go_map, 0); // generate action-goto table
                                                /////////////////////////////////
        IFDEBUG print_action_goto_table(syntax, act_go_map, rows);

        /////////////////////////////////////

        void *(*func[10])(void *, int);

        regist_func;

        long *params = new_array(long, 1024);
        char buffer[1024];
        _setall(params, 0);
        _setarrall(buffer, 0);

        while (gets(buffer))
        {
            syntax_analysis(syntax, act_go_map, buffer, next_lexicon, params, func); // syntax analysis
            printf("\n\n-> %s = %d\n", buffer, *(long *)params);
            _setarrall(buffer, 0);
            _setall(params, 0);
        }
    }

    // cd /home/mayx/code/tool/analyzer/example/calculator
    //  clear&&make clean&&make&&./test
    return 0;
}