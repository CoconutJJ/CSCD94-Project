#include "expression.h"
#include "tokentype.h"

struct Value *make_value(enum TokenType value)
{
        struct Value *v = malloc(sizeof(struct Value));
        v->type = value;
        v->refs = 1;
        return v;
}

void ref_value(struct Value *v) { v->refs++; }

void deref_value(struct Value *v)
{
        v->refs--;

        if (v->refs != 0)
                return;

        if (v->type == STRING)
                free(v->s);

        free(v);
}