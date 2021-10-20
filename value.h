#ifndef VALUE_H
#define VALUE_H
#include "expression.h"
#include "statement.h"
#include "tokentype.h"

struct Value *make_value(enum TokenType value);
void ref_value(struct Value *v);
void deref_value(struct Value *v);

#endif