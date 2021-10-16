#ifndef RETURN_H
#define RETURN_H
#include "expression.h"
#include <stdlib.h>

void push_return_env();
void pop_return_env();
void return_break(struct Value *v);
struct Value *catch_return();
#endif