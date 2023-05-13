#include "syntax.h"
#include "calculator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
//#define DEBUG

#ifdef DEBUG
#define IFDEBUG 
#else
#define IFDEBUG if (0)
#endif
_hashmap *vmap;
int vn = 1;

_vt *simple_vt(char **str_arr, int len)
{

    _vt *res = new_array(_vt, len + 1);
    for (int i = 1; i <= len; i++)
    {
        res[i].type = 0;
        res[i].patten.ch = align_left(str_arr[i - 1], 0);
        hput(vmap, res[i].patten.ch, i);
        // printf("t'%d %s %d'",res[i].patten.ch,res[i].patten.ch,hget(vmap,res[i].patten.ch));
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
    char line[128];
    _syntax *syntax = _new(_syntax);
    int result, prod_arr_size = 0, pos = 0;
    vmap = new_hashmap(64);
    vmap->compare = char_cmp;
    vmap->hash = char_hash;
    // printf("read  %s\n\n",argv[0]);
    if (fp = fopen("D:/a.txt", "r"))
    { //(char*)argv[i],

        // read vt
        fgets(line, sizeof(line), fp);
        int split_len;
        char **simp = split(line, ",", &split_len);
        syntax->vt = simple_vt(simp, split_len);
        syntax->vt_len = split_len;
        // read vn
        fgets(line, sizeof(line), fp);
        syntax->vn = (void **)split(line, ",", &split_len);
        syntax->vn_len = split_len;
        for (int i = 0; i < syntax->vn_len; i++)
        {
            align_left(syntax->vn[i], 0);
            hput(vmap, syntax->vn[i], i + syntax->vt_len);
        }

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
            prod_arr[prod_arr_size].l = (long long)hget(vmap, line);
            ri += 2;
            len = strlen(align_left(ri, 0));

            int *r = new_array(int, len);
            _setall(r, 0);

            pos = 0;
            while (ri[pos])
            {
                r[pos] = (long)hget(vmap, &ri[pos]);
                assert(r[pos] != 0);
                pos++;
            }
            prod_arr[prod_arr_size].r = r;
            prod_arr[prod_arr_size].r_len = pos;
            prod_arr_size++;

            //--------------------------
        }

        fclose(fp);

        syntax->prod = prod_arr;
        syntax->prod_len = prod_arr_size;
        syntax->start = 0;
#ifdef DEBUG
        // check part-------------

        printf("\ncheck\n\n");
        printf("{ \\0 [0]  ");
        int temp = syntax->vt_len;
        for (int i = 1; i <= temp; i++)
        {
            printf(", %s [%d] ", (char *)syntax->vt[i].patten.ch, (long)hget(vmap, syntax->vt[i].patten.ch));
        }
        printf("}\n");

        printf("{ %s [%d] ", syntax->vn[0], hget(vmap, syntax->vn[0]));
        for (int i = 1; i < syntax->vn_len; i++)
        {
            printf(", %s [%d] ", syntax->vn[i], hget(vmap, syntax->vn[i]));
        }
        printf("}\n\n");
        for (int i = 0; i < syntax->prod_len; i++)
        {
            _prod p = syntax->prod[i];
            printf("%d -> ", p.l);
            for (int j = 0; j < p.r_len; j++)
                printf("%d ", p.r[j]);
            printf("\n");
        }
#endif

        // NFA_generate(syntax);
        _act act_go_map[2024];

        LR1_old(syntax, act_go_map); // generate action-goto table
        /////////////////////////////////
       IFDEBUG print_action_goto_table(syntax, act_go_map);

        /////////////////////////////////////

        void (*func[10])(void *);

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

    // cd /home/mayx/code/tool/my_compiler
    //  clear&&make clean&&make&&./test
    return 0;
}