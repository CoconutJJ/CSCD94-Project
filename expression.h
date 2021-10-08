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
        ASSIGNMENT,
        CALL,
        LOGICAL
};

struct Expr {
        enum ExprType type;
        struct Expr* next;
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

struct ExprCall {
        struct Expr obj;
        struct Expr* callee;
        struct Token* paren;
        struct Expr* arguments;
};

struct ExprLiteral {
        struct Expr obj;
        struct Token* token;
};

struct ExprLogical {
        struct Expr obj;
        struct Token* token;
        struct Expr* left;
        struct Expr* right;
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
        struct Value* next;
        union {
                double d;
                char* s;
                bool b;
                struct {
                        struct FunctionStatement* declaration;
                        struct Environment * closure;
                };
        };
        enum TokenType type;
};
#endif