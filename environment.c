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

struct Value* env_get(struct Environment* env, struct Token* name) {
        if (hashtable_has(env->values, name->lexeme)) {
                struct Value* v = hashtable_get(env->values, name->lexeme);

                // preserve hashtable entries, make a copy of data.
                struct Value* new_v = malloc(sizeof(struct Value));

                *new_v = *v;

                return new_v;
        }

        if (env->enclosing != NULL) {
                return env_get(env->enclosing, name);
        }

        runtime_error(name, "Undefined Variable");
        return NULL;
}

struct Environment* env_ancestor(struct Environment* env, int distance) {
        while (distance > 0) {
                env = env->enclosing;
                distance--;
        }

        return env;
}

struct Value* env_get_at(struct Environment* env, int distance, char* name) {
        return hashtable_get(env_ancestor(env, distance)->values, name);
}

void env_assign_at(struct Environment* env, int distance, struct Token* name,
                   struct Value* v) {
        hashtable_set(env_ancestor(env, distance)->values, name->lexeme, v,
                      sizeof(struct Value));
}

void env_assign(struct Environment* env, struct Token* name, struct Value* v) {
        if (hashtable_has(env->values, name->lexeme)) {
                hashtable_set(env->values, name->lexeme, v,
                              sizeof(struct Value));
                return;
        }

        if (env->enclosing != NULL) {
                env_assign(env->enclosing, name, v);
                return;
        }

        runtime_error(name, "Undefined Variable");
}

struct Environment* new_env(struct Environment* env) {
        struct Environment* new = make_env();

        new->enclosing = env;

        return new;
}

void destroy_env(struct Environment* env) {
        hashtable_destroy(env->values);
        free(env);
}