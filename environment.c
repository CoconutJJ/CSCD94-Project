#include <stdlib.h>

#include "./hashtable/hash_table.h"
#include "environment_types.h"
#include "error.h"
#include "expression.h"
#include "tokentype.h"

struct Environment* make_env() {
        struct Environment* env = malloc(sizeof(struct Environment));
        env->values = hashtable_new();
        env->enclosing = NULL;
        return env;
}

void env_define(struct Environment* env, char* name, struct Value* v) {
        hashtable_set(env->values, name, v, sizeof(struct Value));
}

struct Value * env_get(struct Environment* env, struct Token* name) {
        if (hashtable_has(env->values, name->lexeme)) {
                return hashtable_get(env->values, name->lexeme);
        }

        if (env->enclosing != NULL)  {
                return env_get(env->enclosing, name);
        }

        runtime_error(name, "Undefined Variable");
        return NULL;
}

void env_assign(struct Environment* env, struct Token * name, struct Value * v) {

    if (hashtable_has(env->values, name->lexeme)) {
        hashtable_set(env->values, name->lexeme, v, sizeof(struct Value));
        return;
    }

    if (env->enclosing != NULL) {
            env_assign(env->enclosing, name, v);
            return;
    }

    runtime_error(name, "Undefined Variable");

}