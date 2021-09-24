#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "environment.h"
#include "environment_types.h"
#include "error.h"
#include "expression.h"
#include "statement.h"

struct Environment * env;

struct Value* evaluate_expr(struct Expr* expr);
void execute(struct Statement* stmt);
struct Value* make_value(enum TokenType value) {
        struct Value* v = malloc(sizeof(struct Value));
        v->type = value;
        return v;
}

int is_truthy(struct Value* v) {
        switch (v->type) {
        case NUMBER:
                return v->d != 0;
        case TRUE:
        case FALSE:
                return v->b;
        case STRING:
                return 1;
        case NIL:
                return 0;
        default:
                // TODO: unreachable
                break;
        }

        return 0;
}

void check_number_operand(struct Token* operator, struct Value * operand) {
        if (operand->type == NUMBER) return;
        runtime_error(operator, "Operand must be number!");
}

void check_number_operands(struct Token* operator, struct Value * left,
                           struct Value* right) {
        if (left->type == NUMBER && right->type == NUMBER) return;

        runtime_error(operator, "Operands must be numbers!");
}

char* string_concat(char* l, char* r) {
        char* c = malloc(strlen(l) + strlen(r) + 1);
        strcpy(c, l);
        strcat(c, r);
        return c;
}

int is_equal(struct Value* l, struct Value* r) {
        if (l->type == NUMBER && r->type == NUMBER) {
                return (l->d == r->d);
        } else if (l->type == STRING && r->type == STRING) {
                return (strcmp(l->s, r->s) == 0);
        } else {
                return 0;
        }
}

struct Value* visit_literal(struct ExprLiteral* literal) {
        struct Value* v;
        v = make_value(literal->token->type);
        switch (literal->token->type) {
        case NUMBER:
                v->d = *((double*)literal->token->literal);
                break;
        case STRING:
                v->s = literal->token->literal;
                break;
        case TRUE:
                v->b = true;
                break;
        case FALSE:
                v->b = false;
        case NIL:
                break;
        default:
                // TODO: unreachable
                break;
        }

        return v;
}

struct Value * visit_logical(struct ExprLogical * logical) {
        struct Value * left = evaluate_expr(logical->left);

        if (logical->token->type == OR) {
                if (is_truthy(left)) return left;
        } else {
                if (!is_truthy(left)) return left;
        }

        return evaluate_expr(logical->right);
}

struct Value* visit_grouping(struct ExprGrouping* group) {
        struct Value* v = evaluate_expr(group->child);

        return v;
}

struct Value* visit_unary(struct ExprUnr* unary) {
        struct Value* v = evaluate_expr(unary->child);

        switch (unary->token->type) {
        case MINUS:
                check_number_operand(unary->token, v);
                v->d = -(v->d);
                break;
        case BANG:
                // TODO: free string case
                int state = !is_truthy(v);
                v->type = (state == 1) ? TRUE : FALSE;
                v->b = state;
                break;
        default:
                break;
        }
        return v;
}

struct Value* visit_variable(struct ExprVariable* variable) {
        return env_get(env, variable->name);
}

struct Value* visit_binary(struct ExprBin* binary) {
        struct Value* l = evaluate_expr(binary->left);
        struct Value* r = evaluate_expr(binary->right);

        struct Value* v;
        bool b;
        switch (binary->token->type) {
        case MINUS:
                check_number_operands(binary->token, l, r);
                v = make_value(NUMBER);
                v->d = l->d - r->d;
                break;
        case PLUS:
                if (l->type == NUMBER && r->type == NUMBER) {
                        v = make_value(NUMBER);
                        v->d = l->d + r->d;
                } else if (l->type == STRING && r->type == STRING) {
                        v = make_value(STRING);
                        v->s = string_concat(l->s, r->s);
                }
                break;
        case SLASH:
                check_number_operands(binary->token, l, r);
                v = make_value(NUMBER);
                v->d = l->d / r->d;
                break;
        case STAR:
                check_number_operands(binary->token, l, r);
                v = make_value(NUMBER);
                v->d = l->d * r->d;
                break;
        case GREATER:
                check_number_operands(binary->token, l, r);
                b = (l->d > r->d);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        case GREATER_EQUAL:
                check_number_operands(binary->token, l, r);
                b = (l->d >= r->d);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        case LESS:
                check_number_operands(binary->token, l, r);
                b = (l->d < r->d);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        case LESS_EQUAL:
                check_number_operands(binary->token, l, r);
                b = (l->d <= r->d);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        case BANG_EQUAL:
                b = !is_equal(l, r);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        case EQUAL_EQUAL:
                b = is_equal(l, r);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        default:
                break;
        }

        free(l);
        free(r);

        return v;

}

struct Value* evaluate_expr(struct Expr* expr) {
        switch (expr->type) {
        case BINARY:
                return visit_binary((struct ExprBin*)expr);
        case UNARY:
                return visit_unary((struct ExprUnr*)expr);
        case LITERAL:
                return visit_literal((struct ExprLiteral*)expr);
        case GROUPING:
                return visit_grouping((struct ExprGrouping*)expr);
        default:
                break;
        }
        return NULL;
}

void visit_expression_stmt(struct ExpressionStatement* stmt) {
        evaluate_expr(stmt->expr);
}

void visit_print_stmt(struct PrintStatement* stmt) {
        struct Value* v = evaluate_expr(stmt->expr);

        switch (v->type) {
        case NUMBER:
                printf("%f", v->d);
                break;
        case TRUE:
                printf("true");
                break;
        case FALSE:
                printf("false");
                break;
        case STRING:
                printf(v->s);
                break;
        case NIL:
                printf("nil");
                break;
        default:
                // TODO: unreachable
                break;
        }
        free(v);
}

void visit_variable_stmt(struct VariableStatement* stmt) {
        struct Value* v = NULL;
        if (stmt->value) {
                v = evaluate_expr(stmt->value);
        }
        env_define(env, stmt->name->lexeme, v);
        free(v);
}

void execute_block(struct Statement * stmts, struct Environment * e) {
        struct Environment * previous = env;

        env = e;

        while (stmts != NULL) {
                execute(stmts);
                stmts = stmts->next;
        }

        env = previous;
}

void visit_block_stmt(struct BlockStatement * blk) {

        struct Environment * e = make_env();

        e->enclosing = env;

        execute_block(blk->stmts, e);
}

void execute(struct Statement* stmt) {}

void interpret(struct Statement* stmts) {
        env = make_env();
        for (; stmts != NULL; stmts = stmts->next) {
                execute(stmts);
        }
}