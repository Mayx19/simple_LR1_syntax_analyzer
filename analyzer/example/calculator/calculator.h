#include "tool.h"
#define regist_func       \
    func[0] = do_nothing; \
    func[1] = bracket;    \
    func[2] = calculator; \
    func[3] = calculator; \
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
    long *param = params;

    while (_is_blank(str[*pos + len]))
        len++;
    if (str[*pos + len] <= '9' && str[*pos + len] >= '0')
    {
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
    case ')':
        *pos += 1;
        return 3;
    case '-':
        *pos += 1;
        *param = 0;
        return 5;
    case '+':
        *pos += 1;
        *param = 1;
        return 5;
    case '/':
        *pos += 1;
        *param = 2;
        return 4;
    case '*':
        *pos += 1;
        *param = 3;
        return 4;
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
    long *params = param;
    printf(" %d[%d] (%d)", params, pos, params[pos + 1]);
    params[pos] = params[pos + 1];
    return NULL;
}

void *calculator(void *param, int pos)
{
    long *params = param;
    printf(" %d[%d] %d op(%d) %d", params, pos, params[pos],params[pos+1], params[pos + 2]);
    switch (params[pos + 1])
    {
    case 0:
        params[pos] = params[pos] - params[pos + 2];
        break;
    case 1:
        params[pos] = params[pos] + params[pos + 2];
        break;
    case 2:
        params[pos] = params[pos] / params[pos + 2];
        break;
    case 3:
        params[pos] = params[pos] * params[pos + 2];
        break;
    }
    return NULL;
}
