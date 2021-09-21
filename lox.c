#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void run(char *source) {}

void run_file(char *path) {
        FILE *fp = fopen(path, "r");

        fseek(fp, 0, SEEK_END);

        long fileSize = ftell(fp);

        fseek(fp, 0, SEEK_SET);

        char *source = malloc(fileSize * sizeof(char));

        fread(source, fileSize, 1, fp);

        fclose(fp);

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
        if (argc > 1) {
                fprintf(stderr, "Usage: clox [script]");
                exit(EXIT_FAILURE);
        } else if (argc == 1) {
                run_file(argv[1]);
        } else {
                run_prompt();
        }
}
