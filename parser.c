#include <stdlib.h>

#include "error.h"
#include "expression.h"
#include "statement.h"
#include "tokentype.h"
struct Token* head = NULL;
struct Token* current = NULL;
struct Token* prev = NULL;

struct Expr* parse_equality();
struct Statement* parse_statement();

static struct Token* peek() { return current; }

static struct Token* previous() { return prev; }

static int is_at_end() { return peek()->type == END; }

void init_parser(struct Token * h) {
        head = h;
        current = h;
}

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
        return NULL;
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

struct ExprLogical* make_expr_logical() {
        struct ExprLogical* logical = malloc(sizeof(struct ExprLogical));

        logical->obj.type = LOGICAL;
        logical->left = NULL;
        logical->right = NULL;
        logical->token = NULL;

        return logical;
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
                return (struct Expr*)make_expr_literal(previous());
        }

        if (match(IDENTIFIER)) {
                return (struct Expr*)make_expr_variable(previous());
        }

        if (match(LEFT_PAREN)) {
                struct ExprGrouping* group = make_expr_grouping();
                group->child = parse_equality();
                consume(RIGHT_PAREN);

                return (struct Expr*)group;
        }

        return NULL;
}

struct Expr* parse_unary() {
        if (match(BANG) || match(MINUS)) {
                struct Token* op = previous();
                struct Expr* child = parse_unary();

                struct ExprUnr* root = make_expr_unary();

                root->child = child;
                root->token = op;

                return (struct Expr*)root;
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

struct Expr* parse_and() {
        struct Expr* left = parse_equality();

        while (match(AND)) {
                struct Token* op = previous();
                struct Expr* right = parse_equality();
                struct ExprLogical* root = make_expr_logical();
                root->left = left;
                root->token = op;
                root->right = right;
                left = (struct Expr*)root;
        }

        return left;
}

struct Expr* parse_or() {
        struct Expr* left = parse_and();

        while (match(OR)) {
                struct Token* op = previous();
                struct Expr* right = parse_and();
                struct ExprLogical* root = make_expr_logical();
                root->left = left;
                root->token = op;
                root->right = right;
                left = (struct Expr*)root;
        }

        return left;
}

struct Expr* parse_assignment() {
        struct Expr* expr = parse_or();
        if (match(EQUAL)) {
                struct Token* equals = previous();
                struct Expr* value = parse_assignment();

                if (expr->type == VARIABLE) {
                        struct Token* name = ((struct ExprVariable*)expr)->name;

                        return (struct Expr*)make_expr_assignment(name, value);
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

struct BlockStatement* make_block_statement(struct Statement* stmts) {
        struct BlockStatement* b = malloc(sizeof(struct BlockStatement));

        b->obj.type = S_BLK;
        b->obj.next = NULL;
        b->stmts = stmts;

        return b;
}

struct IfStatement* make_if_statement() {
        struct IfStatement* s = malloc(sizeof(struct IfStatement));

        s->obj.type = S_IF;
        s->obj.next = NULL;
        s->condition = NULL;
        s->thenBranch = NULL;
        s->elseBranch = NULL;

        return s;
}

struct PrintStatement* parse_print_statement() {
        struct Expr* value = parse_equality();

        consume(SEMICOLON);

        return make_print_statement(value);
}

struct ExpressionStatement* parse_expression_statement() {
        struct Expr* value = parse_expression();
        consume(SEMICOLON);
        return make_expression_statement(value);
}

struct Statement* parse_block_statement() {
        struct Statement* stmts = NULL;
        struct Statement* end = NULL;
        while (!check(RIGHT_BRACE) && !is_at_end()) {
                if (!stmts) {
                        stmts = parse_statement();
                        end = stmts;
                } else {
                        end->next = parse_statement();
                        end = end->next;
                }
        }

        consume(RIGHT_BRACE);

        return stmts;
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

struct IfStatement* parse_if_statement() {
        struct IfStatement* new_if = make_if_statement();

        consume(LEFT_PAREN);
        new_if->condition = parse_expression();
        consume(RIGHT_PAREN);

        new_if->thenBranch = parse_statement();

        if (match(ELSE)) {
                new_if->elseBranch = parse_statement();
        }

        return new_if;
}

struct Statement* parse_statement() {
        if (match(IF)) return NULL;
        if (match(PRINT)) return (struct Statement*)parse_print_statement();
        if (match(VAR)) return (struct Statement*)parse_variable_statement();
        if (match(LEFT_BRACE))
                return (struct Statement*)make_block_statement(parse_block_statement());

        return (struct Statement*)parse_expression_statement();
}

struct Statement* parse() {
        struct Statement* list = NULL;
        struct Statement* end = NULL;
        while (!is_at_end()) {
                struct Statement* curr = parse_statement();

                if (!list) {
                        list = curr;
                        end = curr;
                } else {
                        end->next = curr;
                        end = end->next;
                }
        }
        return list;
}