#include "environment.h"
#include "expression.h"
#include "interpreter.h"
#include "return.h"
#include "statement.h"
#include <setjmp.h>
#include <stdlib.h>
struct Value *function_call(struct Environment *global, struct Value *callee,
                            struct Value *arguments)
{
        struct Environment *e = new_env(callee->closure);

        struct Token *call_args = callee->declaration->params;

        while (call_args != NULL) {
                env_define(e, call_args->lexeme, arguments);

                call_args = call_args->next;
                arguments = arguments->next;
        }

        struct Value *ret = catch_return();
        if (!ret) {
                execute_block(callee->declaration->body, e);

                /**
                 * NOTE: Below this comment is only reachable if function body
                 * does not contain any return statements, otherwise, the
                 * return_break() will jump back to inside the catch_return()
                 * function which will return a non-NULL pointer to the return
                 * value instead which, of course, will skip this if clause all
                 * together.
                 */

                /**
                 * We need to remove the unused return env created above.
                 */
                pop_return_env();

                ret = malloc(sizeof(struct Value));

                ret->type = NIL;
        }
        destroy_env(e);

        return ret;
}