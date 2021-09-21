#include <stdlib.h>
#include <stdio.h>
#include "tokentype.h"
int hasError = 0;


void report(int line, char * where, char * message) {
        fprintf(stderr, "[line ");
        fprintf(stderr, "%d", line);
        fprintf(stderr, "] Error");
        fprintf(stderr, where);
        fprintf(stderr, ": ");
        fprintf(stderr, message);

        hasError = 1;

}
void error(int line, char * message) {
    report(line, "", message);
}

void runtime_error(struct Token * token, char * message) {
    fprintf(stderr, "Runtime Error: [line %d] %s", token->line, message);
    exit(EXIT_FAILURE);
}