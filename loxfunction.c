#include <stdlib.h>

#include "environment.h"
#include "expression.h"
#include "interpreter.h"
#include "statement.h"

struct Value* function_call(struct Environment* global,
                            struct Value* callee,
                            struct Value* arguments) {
        struct Environment* e = new_env(callee->closure);

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