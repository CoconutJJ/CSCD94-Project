#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "environment.h"
#include "statement.h"
void execute_block(struct Statement* stmts, struct Environment* e);
void interpret(struct Statement* stmts);
void interpreter_resolve(struct Expr* expr, int depth);
void locals_init();
#endif