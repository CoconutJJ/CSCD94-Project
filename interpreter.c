#include "environment.h"
#include "environment_types.h"
#include "error.h"
#include "expression.h"
#include "loxfunction.h"
#include "return.h"
#include "statement.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Environment *global;
struct Environment *env;
struct Locals *locals;

struct Value *evaluate_expr(struct Expr *expr);
void execute(struct Statement *stmt);

struct Locals *make_locals()
{
        struct Locals *l = malloc(sizeof(struct Locals));

        l->h = hashtable_new();

        return l;
}

void obj_id(void *obj_addr, char id[17])
{
        sprintf(id, "%p", obj_addr);
        id[16] = '\0';
}

int locals_get(struct Expr *expr)
{
        char name[8 * 2 + 1];
        obj_id(expr->id, name);

        int *dist = hashtable_get(locals->h, name);

        if (!dist) {
                return -1;
        } else {
                return *dist;
        }
}

void locals_set(struct Expr *expr, int depth)
{
        char name[8 * 2 + 1];
        obj_id(expr->id, name);

        hashtable_set(locals->h, name, &depth, sizeof(int));
}

struct Value *make_value(enum TokenType value)
{
        struct Value *v = malloc(sizeof(struct Value));
        v->type = value;
        return v;
}

int is_truthy(struct Value *v)
{
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
                UNREACHABLE(
                    "is_truthy: executed unreachable default condition");
                break;
        }

        return 0;
}

void check_number_operand(struct Token *operator, struct Value * operand)
{
        if (operand->type == NUMBER)
                return;
        runtime_error(operator, "Operand must be number!");
}

void check_number_operands(struct Token *operator, struct Value * left,
                           struct Value *right)
{
        if (left->type == NUMBER && right->type == NUMBER)
                return;

        runtime_error(operator, "Operands must be numbers!");
}

char *string_concat(char *l, char *r)
{
        char *c = malloc(strlen(l) + strlen(r) + 1);
        strcpy(c, l);
        strcat(c, r);
        return c;
}

int is_equal(struct Value *l, struct Value *r)
{
        if (l->type == NUMBER && r->type == NUMBER) {
                return (l->d == r->d);
        } else if (l->type == STRING && r->type == STRING) {
                return (strcmp(l->s, r->s) == 0);
        } else {
                return 0;
        }
}

struct Value *visit_literal(struct ExprLiteral *literal)
{
        struct Value *v;
        v = make_value(literal->token->type);
        switch (literal->token->type) {
        case NUMBER:
                v->d = *((double *)literal->token->literal);
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
                UNREACHABLE(
                    "visit_literal: executed unreachable default condition");
                break;
        }

        return v;
}

struct Value *visit_logical(struct ExprLogical *logical)
{
        struct Value *left = evaluate_expr(logical->left);

        if (logical->token->type == OR) {
                if (is_truthy(left))
                        return left;
        } else {
                if (!is_truthy(left))
                        return left;
        }

        return evaluate_expr(logical->right);
}

struct Value *visit_grouping(struct ExprGrouping *group)
{
        struct Value *v = evaluate_expr(group->child);

        return v;
}

struct Value *visit_unary(struct ExprUnr *unary)
{
        struct Value *v = evaluate_expr(unary->child);

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
                UNREACHABLE(
                    "visit_unary: executed unreachable default condition");

                break;
        }
        return v;
}

struct Value *look_up_variable(struct Token *name, struct Expr *expr)
{
        int dist = locals_get(expr);
        if (dist >= 0) {
                return env_get_at(env, dist, name->lexeme);
        } else {
                return env_get(global, name);
        }
}

struct Value *visit_variable(struct ExprVariable *variable)
{
        struct Value *v =
            look_up_variable(variable->name, (struct Expr *)variable);

        return v;
}

struct Value *visit_binary(struct ExprBin *binary)
{
        struct Value *l = evaluate_expr(binary->left);
        struct Value *r = evaluate_expr(binary->right);

        struct Value *v;
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
                UNREACHABLE("visit_binary_expr: executed unreachable default "
                            "condition");

                break;
        }

        free(l);
        free(r);

        return v;
}

struct Value *visit_call_expr(struct ExprCall *expr)
{
        struct Value *callee = evaluate_expr(expr->callee);

        struct Expr *arguments = expr->arguments;
        struct Value *head = NULL;
        struct Value *curr = NULL;
        int arg_count = 0;
        while (arguments != NULL) {
                if (!curr) {
                        curr = evaluate_expr(arguments);
                        head = curr;

                } else {
                        curr->next = evaluate_expr(arguments);
                        curr = curr->next;
                }
                arguments = arguments->next;
                arg_count++;
        }

        if (callee->type != FUN) {
                runtime_error(expr->paren,
                              "Can only call functions and classes");
        }

        if (callee->declaration->arity != arg_count) {
                runtime_error(
                    expr->paren,
                    "Function call invoked with incorrect number of arguments");
        }

        struct Value *ret = function_call(global, callee, head);

        free(callee);

        return ret;
}

struct Value *visit_assignment(struct ExprAssignment *stmt)
{
        struct Value *v = evaluate_expr(stmt->value);

        int distance = locals_get((struct Expr *)stmt);

        if (distance >= 0) {
                env_assign_at(env, distance, stmt->name, v);
        } else {
                env_assign(global, stmt->name, v);
        }
        // env_assign(env, stmt->name, v);
        return v;
}

struct Value *evaluate_expr(struct Expr *expr)
{
        switch (expr->type) {
        case BINARY:
                return visit_binary((struct ExprBin *)expr);
        case UNARY:
                return visit_unary((struct ExprUnr *)expr);
        case LITERAL:
                return visit_literal((struct ExprLiteral *)expr);
        case GROUPING:
                return visit_grouping((struct ExprGrouping *)expr);
        case VARIABLE:
                return visit_variable((struct ExprVariable *)expr);
        case ASSIGNMENT:
                return visit_assignment((struct ExprAssignment *)expr);
        case CALL:
                return visit_call_expr((struct ExprCall *)expr);
        case LOGICAL:
                return visit_logical((struct ExprLogical *)expr);
        default:
                UNREACHABLE(
                    "evaluate_expr: executed unreachable default condition");

                break;
        }

        return NULL;
}

void visit_expression_stmt(struct ExpressionStatement *stmt)
{
        struct Value *v = evaluate_expr(stmt->expr);

        if (v)
                free(v);
}

void visit_if_stmt(struct IfStatement *stmt)
{
        if (is_truthy(evaluate_expr(stmt->condition))) {
                execute(stmt->thenBranch);
        } else if (stmt->elseBranch != NULL) {
                execute(stmt->elseBranch);
        }
}

void visit_while_stmt(struct WhileStatement *stmt)
{
        while (is_truthy(evaluate_expr(stmt->condition))) {
                execute((struct Statement *)stmt->body);
        }
}

void visit_function_stmt(struct FunctionStatement *stmt)
{
        struct Value *f = make_value(FUN);
        f->declaration = stmt;
        f->closure = env;
        env_define(env, stmt->name->lexeme, f);
}

void visit_print_stmt(struct PrintStatement *stmt)
{
        struct Value *v = evaluate_expr(stmt->expr);

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
                UNREACHABLE(
                    "visit_print_stmt: executed unreachable default condition");
                break;
        }
        free(v);
}

void visit_declaration_stmt(struct VariableStatement *stmt)
{
        struct Value *v = NULL;
        if (stmt->value) {
                v = evaluate_expr(stmt->value);
        }

        env_define(env, stmt->name->lexeme, v);
        if (v)
                free(v);
}

void execute_block(struct Statement *stmts, struct Environment *e)
{
        struct Environment *previous = env;

        env = e;

        while (stmts != NULL) {
                execute(stmts);
                stmts = stmts->next;
        }

        env = previous;
}

void visit_block_stmt(struct BlockStatement *blk)
{
        struct Environment *e = new_env(env);

        execute_block(blk->stmts, e);

        destroy_env(e);
}

void visit_return_stmt(struct ReturnStatement *stmt)
{
        struct Value *v;
        if (stmt->value) {
                v = evaluate_expr(stmt->value);
        } else {
                v = make_value(NIL);
        }

        return_break(v);
}

void execute(struct Statement *stmt)
{
        switch (stmt->type) {
        case S_BLK:
                visit_block_stmt((struct BlockStatement *)stmt);
                break;
        case S_RET:
                visit_return_stmt((struct ReturnStatement *)stmt);
                break;
        case S_FUN:
                visit_function_stmt((struct FunctionStatement *)stmt);
                break;
        case S_IF:
                visit_if_stmt((struct IfStatement *)stmt);
                break;
        case S_PRINT:
                visit_print_stmt((struct PrintStatement *)stmt);
                break;
        case S_VAR:
                visit_declaration_stmt((struct VariableStatement *)stmt);
                break;
        case S_EXPR:
                visit_expression_stmt((struct ExpressionStatement *)stmt);
                break;
        case S_WHILE:
                visit_while_stmt((struct WhileStatement *)stmt);
                break;
        default:
                UNREACHABLE("execute: executed unreachable default condition");

                break;
        }
}

void interpreter_resolve(struct Expr *expr, int depth)
{
        locals_set(expr, depth);
}

void locals_init() { locals = make_locals(); }

void interpret(struct Statement *stmts)
{
        env = make_env();
        global = env;
        for (; stmts != NULL; stmts = stmts->next) {
                execute(stmts);
        }
        free(env);
}