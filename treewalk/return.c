#include "expression.h"
#include <setjmp.h>
#include <stdlib.h>

struct ReturnEnv {
        jmp_buf ret;
        struct Value *return_value;
        struct Environment * original_env;
        struct ReturnEnv *next;
        struct ReturnEnv *prev;
};

static struct ReturnEnv *head = NULL;
static struct ReturnEnv *curr = NULL;

struct ReturnEnv *make_return_env()
{
        struct ReturnEnv *new = malloc(sizeof(struct ReturnEnv));
        new->next = NULL;
        new->prev = NULL;
        new->return_value = NULL;
        new->original_env = NULL;
        
        return new;
}

struct ReturnEnv * push_return_env()
{

        if (!curr) {
                head = make_return_env();
                curr = head;
        } else {
                curr->next = make_return_env();
                curr->next->prev = curr;
                curr = curr->next;
        }

        return curr;
}

void pop_return_env()
{
        struct ReturnEnv *prev = curr->prev;

        if (prev)
                prev->next = NULL;

        free(curr);
        curr = prev;
}

struct ReturnEnv * get_return_env() {
        return curr;
}