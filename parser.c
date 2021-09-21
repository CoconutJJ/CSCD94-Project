#include <stdlib.h>

#include "expression.h"
#include "tokentype.h"

struct Token* head = NULL;
struct Token* current = NULL;
struct Token* prev = NULL;

static struct Token* peek() { return current; }

static struct Token* previous() { return prev; }

static int is_at_end() { return peek()->type == EOF; }

static struct Token* advance() {
        if (!is_at_end()) {
                prev = current;
                current = current->next;
        }
        return previous();
}

static int check(enum TokenType type) {
        if (is_at_end()) return 0;

        return peek()->type == type;
}

static int match(enum TokenType type) {
        if (check(type)) {
                advance();
                return 1;
        }
        return 0;
}

static struct Token* consume(enum TokenType type) {
        if (check(type)) {
                return advance();
        }
}

struct ExprBin* make_expr_binary() {
        struct ExprBin* bin = malloc(sizeof(struct ExprBin));

        bin->obj.type = BINARY;
        bin->left = NULL;
        bin->right = NULL;
        bin->token = NULL;

        return bin;
}

struct ExprUnr* make_expr_unary() {
        struct ExprUnr* unr = malloc(sizeof(struct ExprUnr));

        unr->obj.type = UNARY;
        unr->child = NULL;
        unr->token = NULL;

        return unr;
}

struct ExprGrouping* make_expr_grouping() {
        struct ExprGrouping* group = malloc(sizeof(struct ExprGrouping));

        group->obj.type = GROUPING;
        group->child = NULL;

        return group;
}

struct ExprLiteral* make_expr_literal(struct Token* token) {
        struct ExprLiteral* lit = malloc(sizeof(struct ExprLiteral));

        lit->obj.type = LITERAL;
        lit->token = token;

        return lit;
}

struct Expr* parse_primary() {
        if (match(FALSE) || match(TRUE) || match(NIL) || match(NUMBER) ||
            match(STRING)) {
                return make_expr_literal(previous());
        }

        if (match(LEFT_PAREN)) {
                struct ExprGrouping* group = make_expr_grouping();
                group->child = parse_equality();
                consume(RIGHT_PAREN);

                return group;
        }
}

struct Expr* parse_unary() {
        if (match(BANG) || match(MINUS)) {
                struct Token* op = previous();
                struct Expr* child = parse_unary();

                struct ExprUnr* root = make_expr_unary();

                root->child = child;
                root->token = op;

                return root;
        }

        return parse_primary();
}

struct Expr* parse_factor() {
        struct Expr* left = parse_unary();

        while (match(MINUS) || match(PLUS)) {
                struct Token* op = previous();
                struct Expr* right = parse_unary();

                struct ExprBin* root = make_expr_binary();

                root->left = left;
                root->right = right;
                root->token = op;

                left = (struct Expr*)root;
        }

        return left;
}

struct Expr* parse_term() {
        struct Expr* left = parse_factor();

        while (match(MINUS) || match(PLUS)) {
                struct Token* op = previous();
                struct Expr* right = parse_factor();

                struct ExprBin* root = make_expr_binary();

                root->left = left;
                root->right = right;
                root->token = op;

                left = (struct Expr*)root;
        }

        return left;
}

struct Expr* parse_comparison() {
        struct Expr* left = parse_term();

        while (match(GREATER) || match(GREATER_EQUAL) || match(LESS) ||
               match(LESS_EQUAL)) {
                struct Token* op = previous();
                struct Expr* right = parse_term();

                struct ExprBin* root = make_expr_binary();

                root->left = left;
                root->right = right;
                root->token = op;

                left = (struct Expr*)root;
        }

        return left;
}

struct Expr* parse_equality() {
        struct Expr* left = parse_comparison();

        while (match(EQUAL_EQUAL) || match(BANG_EQUAL)) {
                struct Token* op = previous();
                struct Expr* right = parse_comparison();

                struct ExprBin* root = make_expr_binary();

                root->left = left;
                root->right = right;
                root->token = op;

                left = (struct Expr*)root;
        }

        return left;
}
