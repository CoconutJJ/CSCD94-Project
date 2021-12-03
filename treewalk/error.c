#include <stdlib.h>
#include <stdio.h>
#include "tokentype.h"
extern int hasError;
void report(int line, char * where, char * message) {
        fprintf(stderr, "[line ");
        fprintf(stderr, "%d", line);
        fprintf(stderr, "] Error");
        fprintf(stderr, where);
        fprintf(stderr, ": ");
        fprintf(stderr, message);
        fprintf(stderr, "\n");
        hasError = 1;

}
void error(int line, char * message) {
    report(line, "", message);
    hasError = 1;
}

void runtime_error(struct Token * token, char * message) {
    fprintf(stderr, "Runtime Error: [line %d] %s", token->line, message);
    exit(EXIT_FAILURE);
}

void UNREACHABLE(char * message) {
    fprintf(stderr, message);
    exit(EXIT_FAILURE);
}