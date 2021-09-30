#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "interpreter.h"
#include "statement.h"

int hasError = 0;
extern struct Token *tokenize(char *src);
extern void init_parser(struct Token *head);
extern struct Statement *parse();

void run(char *source) {
        struct Token *head = tokenize(source);
        init_parser(head);
        struct Statement *p = parse();
        interpret(p);
}

void run_file(char *path) {
        FILE *fp = fopen(path, "r");

        fseek(fp, 0, SEEK_END);

        long fileSize = ftell(fp);

        fseek(fp, 0, SEEK_SET);

        char *source = malloc((fileSize + 1) * sizeof(char));

        fread(source, fileSize, 1, fp);

        fclose(fp);
        source[fileSize] = '\0';

        run(source);

        if (hasError) exit(EXIT_FAILURE);
}

void run_prompt() {
        char buf[500];

        while (1) {
                buf[0] = '\0';
                printf("> ");
                fgets(buf, 500, stdin);
                run(buf);
                hasError = 0;
        }
}

int main(int argc, char **argv) {
        if (argc > 2) {
                fprintf(stderr, "Usage: clox [script]");
                exit(EXIT_FAILURE);
        } else if (argc == 2) {
                run_file(argv[1]);
        } else {
                run_prompt();
        }
}
