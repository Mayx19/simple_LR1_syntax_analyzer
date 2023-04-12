
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"
#define DEBUG
#include "../datrie.h"
// cd ./code/tool/container/test

int main()
{
    datrie *trie = new_datrie();
    trie->chr_num = 12;
    print_trie(trie, 10);

    char c0[] = {2, 0};
    char c1[] = {1, 2, 3, 1, 0};
    char c2[] = {1, 2, 3, 1, 2, 3, 0};
    char c3[] = {3, 1, 1, 3, 0};

    char c4[] = {1, 2, 3, 0};
    char c5[] = {4, 2, 0};
    char c6[] = {3, 3, 1, 0};
    char c7[] = {1, 2, 6, 0};
    char c8[] = {3, 1, 1, 3,3,0};

    datrie_insert(trie, c0);
    print_trie(trie, 20);
    // datrie_insert(trie, c1);
    // print_trie(trie, 20);
    datrie_insert(trie, c2);
    print_trie(trie, 20);
    // datrie_insert(trie, c3);
    // print_trie(trie, 20);
    datrie_insert(trie, c4);
    print_trie(trie, 20);
    // datrie_insert(trie, c5);
    // print_trie(trie, 20);
    datrie_insert(trie, c6);
    print_trie(trie, 20);
    datrie_insert(trie, c7);
    print_trie(trie, 20);

    printf(" = %d\n",datrie_find(trie,c0));
    printf(" = %d\n",datrie_find(trie,c1));
    printf(" = %d\n",datrie_find(trie,c2));
    printf(" = %d\n",datrie_find(trie,c3));
    printf(" = %d\n",datrie_find(trie,c4));
    printf(" = %d\n",datrie_find(trie,c5));
    printf(" = %d\n",datrie_find(trie,c6));
    printf(" = %d\n",datrie_find(trie,c7));
    printf(" = %d\n",datrie_find(trie,c8));


}