#include "tokentype.h"
#include <stdbool.h>
enum ExprType {

        BINARY,
        UNARY,
        LITERAL,
        GROUPING
};

enum ValueType {
        NUM,
        STR,
        BOOL,
        NIL
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

struct Value {
        enum TokenType type;
        union {
                double d;
                char * s;
                bool b;
        };
};

// struct ExprCall {

// }
