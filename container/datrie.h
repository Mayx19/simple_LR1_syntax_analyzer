
struct datrie
{
    int base_capacity, tail_capacity, mark, chr_num;
    int *base;
    int *check;
    char *tail;
} typedef datrie;

datrie *new_datrie();

void print_trie(datrie *trie, int n);

void datrie_add_tail(datrie *trie, int pos, char *str);

void datrie_add_to_leaf(datrie *trie, int parent, char *c);

int datrie_children_count(datrie *trie, int moved_parent, int *children);

int datrie_move(datrie *trie, int moved_parent, int *children, int children_count);

void datrie_insert(datrie *trie, char *str);

int datrie_find(datrie *trie, char *str);

void datrie_trim(datrie *trie);
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
                   parent = tire->check[j] - min_root;
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