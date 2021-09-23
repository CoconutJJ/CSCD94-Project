
#ifndef ENV_H
#define ENV_H
#include "environment_types.h"
#include "expression.h"
#include "tokentype.h"

struct Environment* make_env();
void env_define(struct Environment* env, char* name, struct Value* v);
struct Value *env_get(struct Environment* env, struct Token* name);
#endif