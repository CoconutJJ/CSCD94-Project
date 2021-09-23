
#ifndef STATEMENT_H
#define STATEMENT_H
#include "tokentype.h"
#include "expression.h"

enum StatementType {
    S_PRINT,
    S_IF,
    S_WHILE,
    S_FOR,
    S_EXPR,
    S_VAR
};

struct Statement {
    enum StatementType type;
    struct Statement * next;
};

struct PrintStatement {
    struct Statement obj;
    struct Expr * expr;
};

struct ExpressionStatement {
    struct Statement obj;
    struct Expr * expr;
};

struct VariableStatement {
    struct Statement obj;
    struct Token * name;
    struct Expr * value;
};
#endif