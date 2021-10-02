#include <stdlib.h>

#include "environment.h"
#include "expression.h"
#include "statement.h"
#include "interpreter.h"
struct Value* function_call(struct Environment* env, struct Value* callee,
                            struct Value* arguments) {
        struct Environment* e = new_env(env);

        struct Token* call_args = callee->declaration->params;

        while (call_args != NULL) {

            env_define(e, call_args->lexeme, arguments);

            call_args = call_args->next;
            arguments = arguments->next;

        }

        execute_block(callee->declaration->body, e);

        destroy_env(e);

        return NULL;

}