
#ifndef STATEMENT_H
#define STATEMENT_H
#include "expression.h"
#include "tokentype.h"

enum StatementType { S_PRINT, S_IF, S_WHILE, S_FOR, S_EXPR, S_VAR, S_BLK };

struct Statement {
        enum StatementType type;
        struct Statement* next;
};

struct PrintStatement {
        struct Statement obj;
        struct Expr* expr;
};

struct ExpressionStatement {
        struct Statement obj;
        struct Expr* expr;
};

struct VariableStatement {
        struct Statement obj;
        struct Token* name;
        struct Expr* value;
};

struct BlockStatement {
        struct Statement obj;
        struct Statement* stmts;
};

struct IfStatement {
        struct Statement obj;
        struct Expr* condition;
        struct Statement* thenBranch;
        struct Statement* elseBranch;
};

struct WhileStatement {
        struct Statement obj;
        struct Expr* condition;
        struct Statment* body;
};

struct ForStatement {

};


#endif