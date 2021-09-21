#include <stdlib.h>

#include "error.h"
#include "expression.h"
#include "string.h"

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

struct Value * visit_grouping(struct ExprGrouping * group) {

        struct Value * v = evaluate_expr(group->child);

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

struct Value* visit_binary(struct ExprBin* binary) {
        struct Value* l = evaluate_expr(binary->left);
        struct Value* r = evaluate_expr(binary->right);

        struct Value* v;

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
                bool b = (l->d > r->d);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        case GREATER_EQUAL:
                check_number_operands(binary->token, l, r);
                bool b = (l->d >= r->d);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        case LESS:
                check_number_operands(binary->token, l, r);
                bool b = (l->d < r->d);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        case LESS_EQUAL:
                check_number_operands(binary->token, l, r);
                bool b = (l->d <= r->d);
                v = make_value(b ? TRUE : FALSE);
                v->b = b;
                break;
        case BANG_EQUAL:
                bool not_eq = !is_equal(l, r);
                v = make_value(not_eq ? TRUE : FALSE);
                v->b = not_eq ;
                break;
        case EQUAL_EQUAL:
                bool eq = !is_equal(l, r);
                v = make_value(eq ? TRUE : FALSE);
                v->b = eq;
                break;
        default:
                break;
        }
}

struct Value* evaluate_expr(struct Expr* expr) {
        switch (expr->type) {
        case BINARY:
                return visit_binary((struct ExprBin *)expr);
        case UNARY:
                return visit_unary((struct ExprUnr *)expr);
        case LITERAL:
                return visit_literal((struct ExprLiteral *)expr);
        case GROUPING:
                return visit_grouping((struct ExprGrouping *)expr);
        default:
                break;
        }
}
