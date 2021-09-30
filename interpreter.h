#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "environment.h"
#include "statement.h"
void execute_block(struct Statement* stmts, struct Environment* e);
void interpret(struct Statement* stmts);
#endif