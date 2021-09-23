#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <stdbool.h>

#include "tokentype.h"
enum ExprType {

        BINARY,
        UNARY,
        LITERAL,
        GROUPING,
        VARIABLE,
        ASSIGNMENT
};

struct Expr {
        enum ExprType type;
};

struct ExprBin {
        struct Expr obj;
        struct Token* token;
        struct Expr* left;
        struct Expr* right;
};

struct ExprUnr {
        struct Expr obj;
        struct Token* token;
        struct Expr* child;
};

struct ExprLiteral {
        struct Expr obj;
        struct Token* token;
};

struct ExprGrouping {
        struct Expr obj;
        struct Expr* child;
};

struct ExprVariable {
        struct Expr obj;
        struct Token* name;
};

struct ExprAssignment {
        struct Expr obj;
        struct Token* name;
        struct Expr* value;
};

struct Value {
        enum TokenType type;
        union {
                double d;
                char* s;
                bool b;
        };
};

// struct ExprCall {

// }
#endif