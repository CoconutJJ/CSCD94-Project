#include <stdlib.h>

#include "expression.h"
#include "statement.h"
#include "tokentype.h"
#include "error.h"
struct Token* head = NULL;
struct Token* current = NULL;
struct Token* prev = NULL;

struct Expr* parse_equality();

static struct Token* peek() { return current; }

static struct Token* previous() { return prev; }

static int is_at_end() { return peek()->type == END; }

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

struct ExprVariable* make_expr_variable(struct Token* name) {
        struct ExprVariable* var = malloc(sizeof(struct ExprVariable));

        var->obj.type = VARIABLE;
        var->name = name;

        return var;
}

struct ExprAssignment* make_expr_assignment(struct Token* name,
                                            struct Expr* value) {
        struct ExprAssignment* ass = malloc(sizeof(struct ExprAssignment));

        ass->obj.type = ASSIGNMENT;
        ass->name = name;
        ass->value = value;

        return ass;
}

struct Expr* parse_primary() {
        if (match(FALSE) || match(TRUE) || match(NIL) || match(NUMBER) ||
            match(STRING)) {
                return make_expr_literal(previous());
        }

        if (match(IDENTIFIER)) {
                return make_expr_variable(previous());
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

struct Expr* parse_assignment() {
        struct Expr* expr = parse_equality();
        if (match(EQUAL)) {
                struct Token* equals = previous();
                struct Expr* value = parse_assignment();

                if (expr->type == VARIABLE) {
                        struct Token* name = ((struct ExprVariable*)expr)->name;

                        return make_expr_assignment(name, value);
                }

                error(equals->line, "Invalid Assignment target!");
        }

        return expr;
}

struct Expr* parse_expression() {
        return parse_assignment();
}

struct PrintStatement* make_print_statement(struct Expr* expr) {
        struct PrintStatement* p = malloc(sizeof(struct PrintStatement));

        p->expr = expr;
        p->obj.type = S_PRINT;
        p->obj.next = NULL;
        return p;
}

struct ExpressionStatement* make_expression_statement(struct Expr* expr) {
        struct ExpressionStatement* e =
            malloc(sizeof(struct ExpressionStatement));

        e->expr = expr;
        e->obj.type = S_EXPR;
        e->obj.next = NULL;

        return e;
}

struct VariableStatement* make_variable_statement(struct Token* name,
                                                  struct Expr* value) {
        struct VariableStatement* v = malloc(sizeof(struct VariableStatement));

        v->obj.type = S_VAR;
        v->obj.next = NULL;
        v->name = name;
        v->value = value;

        return v;
}

struct PrintStatement* parse_print_statement() {
        struct Expr* value = parse_equality();

        consume(SEMICOLON);

        return make_print_statement(value);
}

struct ExpressionStatement* parse_expression_statement() {
        struct Expr* value = parse_equality();
        consume(SEMICOLON);
        return make_expression_statement(value);
}

struct VariableStatement* parse_variable_statement() {
        struct Token* name = consume(IDENTIFIER);

        struct Expr* value = NULL;

        if (match(EQUAL)) {
                value = parse_equality();
        }
        consume(SEMICOLON);

        return make_variable_statement(name, value);
}

struct Statement* parse_statement() {
        struct Statement* list = NULL;
        struct Statement* end = NULL;
        struct Statement* curr = NULL;
        while (!is_at_end()) {
                if (match(PRINT))
                        curr = parse_print_statement();
                else if (match(VAR))
                        curr = parse_variable_statement();
                else {
                }

                if (!list) {
                        list = curr;
                        end = curr;
                } else {
                        end->next = curr;
                        end = end->next;
                }
        }

        return parse_expression_statement();
}