#include <stdlib.h>

#include "error.h"
#include "expression.h"
#include "hashtable/hash_table.h"
#include "hashtable/hashtable_types.h"
#include "interpreter.h"
#include "resolve_types.h"
#include "statement.h"
static struct Scope* head = NULL;
static struct Scope* scopes = NULL;
static int scope_size = 0;

void resolve_expr(struct Expr* expr);
void resolve_stmt(struct Statement* stmt);

struct Scope* make_scope() {
        struct Scope* scope = malloc(sizeof(struct Scope));
        scope->h = hashtable_new();
        scope->next = NULL;
        scope->prev = NULL;
        return scope;
}

void destroy_scope(struct Scope* scope) {
        hashtable_destroy(scope->h);
        free(scope);
}

void scope_set(char* lexeme, int set) {
        hashtable_set(scopes->h, lexeme, &set, sizeof(int));
}

int scope_get(char* lexeme) {
        int* state = hashtable_get(scopes->h, lexeme);

        if (!state) return -1;

        return *state;
}

int scope_contains(struct Scope* scope, char* lexeme) {
        return hashtable_has(scope->h, lexeme);
}

void scope_begin() {
        if (!scopes) {
                scopes = make_scope();
                head = scopes;
                return;
        }

        scopes->next = make_scope();
        scopes->next->prev = scopes;
        scopes = scopes->next;

        scope_size++;
}

void scope_end() {
        struct Scope* curr = scopes;

        scopes = scopes->prev;

        if (scopes) scopes->next = NULL;

        destroy_scope(curr);
        scope_size--;
}

int empty_scope() { return scopes == NULL; }

void resolve_statements(struct Statement* stmt) {
        while (stmt) {
                resolve_stmt(stmt);
                stmt = stmt->next;
        }
}

void declare(struct Token* name) {
        if (empty_scope()) return;

        if (scope_contains(scopes, name->lexeme)) {
                runtime_error(
                    name,
                    "Already a variable with this name in the current scope");
        }

        scope_set(name->lexeme, 0);
}

void define(struct Token* name) {
        if (empty_scope()) return;

        scope_set(name->lexeme, 1);
}

void resolve_local(struct Expr* expr, struct Token* name) {
        struct Scope* curr = scopes;
        int i = scope_size - 1;
        while (curr) {
                if (scope_contains(curr, name->lexeme)) {
                        interpreter_resolve(expr, scope_size - 1 - i);
                        return;
                }
                i--;
                curr = curr->prev;
        }
}

void resolve_block_stmt(struct BlockStatement* stmt) {
        scope_begin();
        resolve_statements(stmt->stmts);
        scope_end();
}

void resolve_expression_stmt(struct ExpressionStatement* stmt) {
        resolve_expr(stmt->expr);
}

void resolve_function(struct FunctionStatement* stmt) {
        scope_begin();
        struct Token* params = stmt->params;
        while (params) {
                declare(params);
                define(params);
                params = params->next;
        }
        resolve_statements(stmt->body);
        scope_end();
}

void resolve_function_stmt(struct FunctionStatement* stmt) {
        declare(stmt->name);
        define(stmt->name);

        resolve_function(stmt);
}

void resolve_if_stmt(struct IfStatement* stmt) {
        resolve_expr(stmt->condition);
        resolve_stmt(stmt->thenBranch);

        if (stmt->elseBranch) resolve_stmt(stmt->elseBranch);
}

void resolve_print_stmt(struct PrintStatement* stmt) {
        resolve_expr(stmt->expr);
}

void resolve_return_stmt(struct ReturnStatement* stmt) {
        if (stmt->value) resolve_expr(stmt->value);
}

void resolve_var_stmt(struct VariableStatement* stmt) {
        declare(stmt->name);
        if (stmt->value) {
                resolve_expr(stmt->value);
        }

        define(stmt->name);
}

void resolve_while_stmt(struct WhileStatement* stmt) {
        resolve_expr(stmt->condition);
        resolve_stmt(stmt->body);
}

void resolve_assign_expr(struct ExprAssignment* assign) {
        resolve_expr(assign->value);
        resolve_local((struct Expr*)assign, assign->name);
}

void resolve_binary_expr(struct ExprBin* bin) {
        resolve_expr(bin->left);
        resolve_expr(bin->right);
}

void resolve_call_expr(struct ExprCall* call) {
        resolve_expr(call->callee);

        struct Expr* args = call->arguments;

        while (args) {
                resolve_expr(args);
                args = args->next;
        }
}

void resolve_grouping_expr(struct ExprGrouping* group) {
        resolve_expr(group->child);
}

void resolve_literal_expr(struct ExprLiteral* lit) {
        // we don't do anything.
        return;
}

void resolve_logical_expr(struct ExprLogical* log) {
        resolve_expr(log->left);
        resolve_expr(log->right);
}

void resolve_unary_expr(struct ExprUnr* unr) { resolve_expr(unr->child); }

void resolve_var_expr(struct ExprVariable* expr) {
        if (!empty_scope() && scope_get(expr->name->lexeme) == 0) {
                runtime_error(
                    expr->name,
                    "Can't read local variable in its own initializer");
        }

        resolve_local((struct Expr*)expr, expr->name);
}

void resolve_expr(struct Expr* expr) {
        switch (expr->type) {
        case BINARY:
                resolve_binary_expr((struct ExprBin*)expr);
                break;
        case UNARY:
                resolve_unary_expr((struct ExprUnr*)expr);
                break;
        case LITERAL:
                resolve_literal_expr((struct ExprLiteral*)expr);
                break;
        case GROUPING:
                resolve_grouping_expr((struct ExprGrouping*)expr);
                break;
        case VARIABLE:
                resolve_var_expr((struct ExprVariable*)expr);
                break;
        case ASSIGNMENT:
                resolve_assign_expr((struct ExprAssignment*)expr);
                break;
        case CALL:
                resolve_call_expr((struct ExprCall*)expr);
                break;
        case LOGICAL:
                resolve_logical_expr((struct ExprLogical*)expr);
                break;
        default:
                break;
        }
}

void resolve_stmt(struct Statement* stmt) {
        switch (stmt->type) {
        case S_BLK:
                resolve_block_stmt((struct BlockStatement*)stmt);
                break;
        case S_FUN:
                resolve_function_stmt((struct FunctionStatement*)stmt);
                break;
        case S_IF:
                resolve_if_stmt((struct IfStatement*)stmt);
                break;
        case S_PRINT:
                resolve_print_stmt((struct PrintStatement*)stmt);
                break;
        case S_VAR:
                resolve_var_stmt((struct VariableStatement*)stmt);
                break;
        case S_EXPR:
                resolve_expression_stmt((struct ExpressionStatement*)stmt);
                break;
        case S_WHILE:
                resolve_while_stmt((struct WhileStatement*)stmt);
                break;
        default:
                break;
        }
}