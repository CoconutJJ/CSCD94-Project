#ifndef ERROR_H
#define ERROR_H
#include "tokentype.h"
void report(int line, char* where, char* message);
void error(int line, char* message);
void runtime_error(struct Token* token, char* message);
#endif