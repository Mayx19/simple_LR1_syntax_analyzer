//#include "calculator.h"
#include "regular_expression.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#define DEBUG

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
    // printf("read  %s\n\n",argv[0]);//(char*)argv[i],
    if (fp = fopen("./a.txt", "r"))
    {

        // read vt
        fgets(line, sizeof(line), fp);
        int split_len;
        char **simp = split(line, "|", &split_len);
        syntax->vt = simple_v(simp, split_len, 0);
        syntax->vt_len = split_len + 1;
        // read vn
        fgets(line, sizeof(line), fp);
        simp = split(line, "|", &split_len);
        syntax->vn = simple_v(simp, split_len, 1);
        syntax->vn_len = split_len;

        _prod *prod_arr = new_arr(_prod, 32);
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
        _act act_go_map1[4024] = {0};
        _act act_go_map2[4024] = {0};
        uint rows=LR1(syntax, act_go_map1,0); // generate action-goto table

        vt_map = new_hashmap(64);
        vt_map->compare = char_cmp;
        vt_map->hash = char_hash;

        /////////////////////////////////
        print_action_goto_table(syntax, act_go_map1,rows);

        ///////////////////////////////////

        void *(*func[12])(void *, int);

        regist_func;

        long *params = new0arr(long, 1024);
        // bug "a((ba)*a)*ba,a*b,a([1,3]a)*";
        const char *pattern = "(b*)*c(a*)*";//
        _init_();
        syntax_analysis(syntax, act_go_map1, pattern, next_lexicon, params, func); // syntax analysis
                                                                                   //   printf("\n\n-> %s = %d\n", buffer, *(long *)params);
        printf("----------------------\n");
        printf("syntax->start = %d\n", syn->start);
        for (int i = 0; i < syn->prod_len; i++)
        {
            printf("ID:%d\t", i);
            print_prod_t(syn, &syn->prod[i], -1);
            printf("\n");
        }

        //////////////////////////////////
        rows=LR1(syn, act_go_map2,1);

        print_action_goto_table(syn, act_go_map2,rows);
        char buffer[1024];
        int pos = 0;
        lexical_temp *lexical_t = init_lexical_temp(syn, act_go_map2,rows);
        while (gets(buffer))
        {
            pos = 0;
            printf("\n\nlexical:%d\n", lexical_match_first(syn, act_go_map2, lexical_t, buffer, &pos));
            _setarrall(buffer, 0);
        }
    }

    // cd /home/mayx/code/tool/analyzer/example/regular_expression
    //  clear&&make clean&&make&&./test
    //      display curr
    //      display state[curr_input.state_top][0]
    //      display state[curr_input.state_top+1][1]
    return 0;
}