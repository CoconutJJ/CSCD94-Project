#include "expression.h"
#include <setjmp.h>
#include <stdlib.h>

struct ReturnEnv {
        jmp_buf ret;
        struct Value *return_value;
        struct ReturnEnv *next;
        struct ReturnEnv *prev;
};

struct ReturnEnv *head = NULL;
struct ReturnEnv *curr = NULL;

struct ReturnEnv *make_return_env()
{
        struct ReturnEnv *new = malloc(sizeof(struct ReturnEnv));
        new->next = NULL;
        new->prev = NULL;
        new->return_value = NULL;

        return new;
}

void push_return_env()
{

        if (!curr) {
                head = make_return_env();
                curr = head;
        } else {
                curr->next = make_return_env();
                curr->next->prev = curr;
                curr = curr->next;
        }
}

void pop_return_env()
{
        struct ReturnEnv *prev = curr->prev;

        if (prev)
                prev->next = NULL;

        free(curr);
        curr = prev;
}

void return_break(struct Value *v)
{
        curr->return_value = v;
        longjmp(curr->ret, 1);
}

struct Value *catch_return()
{

        /**
         * Push the return environment onto the stack.
         *
         * Remember that setjmp returns twice, once on the intial call to
         * save the registers and the next time on call longjmp.
         */
        push_return_env();

        if (setjmp(curr->ret) == 0)
                return NULL;

        /**
         * We only reach here if a return statement was hit.
         * Grab the return value, remove this return env and return it.
         */
        struct Value *ret = curr->return_value;

        pop_return_env();

        return ret;
}
