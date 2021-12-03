
#ifndef ENV_H
#define ENV_H
#include "environment_types.h"
#include "expression.h"
#include "tokentype.h"

struct Environment* make_env();
void env_define(struct Environment* env, char* name, struct Value* v);
struct Value* env_get(struct Environment* env, struct Token* name);
void env_assign(struct Environment* env, struct Token* name, struct Value* v);
struct Environment* new_env(struct Environment* env);
void destroy_env(struct Environment* env);
struct Value* env_get_at(struct Environment* env, int distance, char* name);
void env_assign_at(struct Environment* env, int distance, struct Token* name,
                   struct Value* v);
#endif