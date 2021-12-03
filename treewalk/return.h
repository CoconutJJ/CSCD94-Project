#ifndef RETURN_H
#define RETURN_H
#include "expression.h"
#include "environment.h"
#include <setjmp.h>
#include <stdlib.h>
struct ReturnEnv {
        jmp_buf ret;
        struct Value *return_value;
        struct Environment * original_env;
        struct ReturnEnv *next;
        struct ReturnEnv *prev;
};
struct ReturnEnv *push_return_env();
void pop_return_env();
struct ReturnEnv *get_return_env();
#endif