#include "tool.h"
#define regist_func       \
    func[0] = do_nothing; \
    func[1] = bracket;    \
    func[2] = mutiply;    \
    func[3] = add;        \
    func[4] = do_nothing;

// i,(,),*,+
// Z,A
// Z->A
// A->(A)
// A->A*A
// A->A+A
// A->i
int next_lexicon(const char *str, int *pos, void *params)
{
    int len = 0;
    while (_is_blank(str[*pos + len]))
        len++;
    if (str[*pos + len] <= '9' && str[*pos + len] >= '0')
    {
        long *param = params;
        int res = 0;
        while (str[*pos + len] <= '9' && str[*pos + len] >= '0')
        {
            res = res * 10 + str[*pos + len] - '0';
            len++;
        }
        *pos += len;
        *param = res;
        return 1;
    }
    switch (str[*pos + len])
    {
    case '(':
        *pos += 1;
        return 2;
        break;
    case ')':
        *pos += 1;
        return 3;
        break;
    case '*':
        *pos += 1;
        return 4;
        break;
    case '-':
        *pos += 1;
        return 5;
        break;
    case '+':
        *pos += 1;
        return 5;
        break;
    case '/':
        *pos += 1;
        return 7;
        break;
    default:
        return 0;
        break;
    }

    return 0;
}

void *do_nothing(void *param, int pos)
{
    return NULL;
}
void *bracket(void *param, int pos)
{
    long *params = param + pos;
    *params = params[1];
    return NULL;
}

void *add(void *param, int pos)
{
    long *params = param + pos;
    printf(" %d + %d = %d ", params[0], params[2], (params[0] + params[2]));
    *params = (params[0] + params[2]);
    return NULL;
}

void *minus(void *param, int pos)
{
    long *params = param + pos;
    printf(" %d - %d = %d ", params[0], params[2], params[0] - params[2]);
    *params = (params[0] - params[2]);
    return NULL;
}

void *mutiply(void *param, int pos)
{
    long *params = param;
    printf(" %d[%d] %d * %d = %d ", params, pos, params[pos], params[pos + 2], (params[pos] * params[pos + 2]));
    params[pos]  = params[pos] * params[pos + 2];
    return NULL;
}

void *divide(void *param, int pos)
{
    long *params = param;
    printf(" %d[%d] %d / %d = %d ", params, pos, params[pos], params[pos + 2], (params[pos] / params[pos + 2]));
    params[pos]  = params[pos] / params[pos + 2];
    return NULL;
}