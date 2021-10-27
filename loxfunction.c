#include "environment.h"
#include "expression.h"
#include "interpreter.h"
#include "return.h"
#include "statement.h"
#include <setjmp.h>
#include <stdlib.h>

extern struct Environment *env;

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
        
        struct ReturnEnv *ret = push_return_env();

        struct Value *ret_value = NULL;

        if (setjmp(ret->ret) == 0) {

                /**
                 * Capture the environment before executing function body.
                 */
                ret->original_env = env;
                execute_block(callee->declaration->body, e);

                /**
                 * NOTE: Below this comment is only reachable if function body
                 * does not contain any return statements, otherwise, the
                 * longjmp() will jump back to inside the setjmp() function
                 * which will return 1 and will execute the else clause instead.
                 */

                ret_value = malloc(sizeof(struct Value));
                ret_value->type = NIL;

        } else {

                ret_value = ret->return_value;
        }


        /**
         * NOTE: Since return statements can be nested many layers deep, there
         * can be a chain of environments needing to be freed that were not
         * freed because the vist_block_stmt() recursive calls were never
         * "completed".
         *
         * Destroy all environments that were created by this up to but not
         * including the closure environment
         */
        while (env != callee->closure) {
                destroy_env(env);
                env = env->enclosing;
        }

        /**
         * Reset the environment to what it was before the function was
         * called.
         */
        env = ret->original_env;

        pop_return_env();
        
        return ret_value;
}