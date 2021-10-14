#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "tokentype.h"
#define IS_KEYWORD(src, match, token)              \
        do {                                       \
                if (strcmp((src), (match)) == 0) { \
                        return (token);            \
                }                                  \
        } while (0)

static struct Token *head = NULL;
static struct Token *end = NULL;

static int c = 0;
static int line = 1;
static char *code = NULL;

static int is_alpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int is_num(char c) { return (c >= '0' && c <= '9'); }

static void advance() { c++; }

static char peek() { return code[c]; }

static int at_end() { return peek() == '\0'; }

static int match(char m) {
        if (peek() == m) {
                advance();
                return 1;
        }

        return 0;
}

struct Token *make_token(enum TokenType token) {
        struct Token *t = malloc(sizeof(struct Token));

        if (!t) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        t->type = token;
        t->next = NULL;
        t->lexeme = NULL;
        t->line = -1;
        t->literal = NULL;

        return t;
}

void _add_token(struct Token *t) {
        if (head == NULL) {
                head = t;
                end = t;
        } else {
                end->next = t;
                end = end->next;
        }
        t->line = line;
}

void add_token_id(enum TokenType token, char *s) {
        struct Token *t = make_token(token);

        t->lexeme = malloc(strlen(s) + 1);

        if (!t->lexeme) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        strcpy(t->lexeme, s);

        _add_token(t);
}

void add_token_str(enum TokenType token, char *s) {
        struct Token *t = make_token(token);

        t->literal = malloc(strlen(s) + 1);

        if (!t->literal) {
                perror("malloc");
                exit(EXIT_FAILURE);
        }

        strcpy(t->literal, s);

        _add_token(t);
}

void add_token_dbl(enum TokenType token, double d) {
        struct Token *t = make_token(token);

        t->literal = malloc(sizeof(double));

        *((double *)(t->literal)) = d;

        _add_token(t);
}

void add_token(enum TokenType token, void *literal) {
        struct Token *t = make_token(token);
        t->type = token;
        t->literal = literal;
        _add_token(t);
}

void unescape_string(char *str, char *buf) {
        char *start = str;

        /**
         * Employ a cursor that will always point to the next byte we can write
         * to in buf.
         */
        char *cursor = buf;

        char *esc;

        while ((esc = strchr(start, '\\')) != NULL) {
                /**
                 * Only copy bytes from previous upto but not including the
                 * backslash
                 */
                strncpy(cursor, start, esc - start);
                cursor += (esc - start);

                /**
                 * If *(esc + 1) is the null byte, then it will be an invalid
                 * escape sequence since this implies '\' is the last character
                 * in the string and there is no '\' before it to esape it.
                 *
                 * Handled safely by default case
                 */

                switch (*(esc + 1)) {
                case 'a':
                        *cursor = 0x07;
                        break;
                case 'b':
                        *cursor = 0x08;
                        break;
                case 'e':
                        *cursor = 0x1B;
                        break;
                case 'f':
                        *cursor = 0x1C;
                        break;
                case 'n':
                        *cursor = 0x0A;
                        break;
                case 'r':
                        *cursor = 0x0D;
                        break;
                case 'v':
                        *cursor = 0x0B;
                        break;
                case '\\':
                        *cursor = 0x5C;
                        break;
                case '\'':
                        *cursor = 0x27;
                        break;
                case '\"':
                        *cursor = 0x22;
                        break;
                case '\?':
                        *cursor = 0x3F;
                        break;
                default:
                        error(line, "invalid escape sequence");
                        break;
                }

                cursor++;

                /**
                 * Increment esc pointer by 2, one for escape code and another
                 * to mark start of string. Set result to start pointer so next
                 * iteration start search from after the escape sequence.
                 */
                start = esc + 2;
        }

        strcpy(cursor, start);
}

enum TokenType keyword(char *s) {
        IS_KEYWORD(s, "and", AND);
        IS_KEYWORD(s, "class", CLASS);
        IS_KEYWORD(s, "else", ELSE);
        IS_KEYWORD(s, "false", FALSE);
        IS_KEYWORD(s, "for", FOR);
        IS_KEYWORD(s, "if", IF);
        IS_KEYWORD(s, "nil", NIL);
        IS_KEYWORD(s, "or", OR);
        IS_KEYWORD(s, "print", PRINT);
        IS_KEYWORD(s, "return", RETURN);
        IS_KEYWORD(s, "super", SUPER);
        IS_KEYWORD(s, "this", THIS);
        IS_KEYWORD(s, "true", TRUE);
        IS_KEYWORD(s, "var", VAR);
        IS_KEYWORD(s, "while", WHILE);
        IS_KEYWORD(s, "async", ASYNC);
        IS_KEYWORD(s, "fun", FUN);
        return -1;
}

void add_identifier() {
        int end = c;

        while (is_alpha(code[end]) || is_num(code[end])) end++;

        char ident[end - c + 1];

        int i = 0;

        while (is_alpha(peek()) || is_num(peek())) {
                ident[i] = peek();
                advance();
                i++;
        }

        ident[i] = '\0';

        enum TokenType t = keyword(ident);

        if (t == -1)
                add_token_id(IDENTIFIER, ident);
        else
                add_token(t, NULL);
}

void add_number() {
        int end = c;
        int is_dbl = 0;
        while (is_num(code[end]) || code[end] == '.') {
                if (code[end] == '.') {
                        if (!is_dbl)
                                is_dbl = 1;
                        else
                                break;
                }
                end++;
        }
        int num_len = end - c + 1;

        char num[num_len];

        int i = 0;

        while (i < num_len - 1) {
                num[i] = peek();
                advance();
                i++;
        }

        num[num_len] = '\0';

        double *d = malloc(sizeof(double));

        *d = strtod(num, NULL);

        add_token(NUMBER, d);
}

void add_string() {
        advance();
        int end = c;
        while (code[end] != '"') {
                end++;

                if (!code[end]) {
                        error(line, "unexpected end of string");
                }
        }

        char str[end - c + 1];
        int str_len = end - c;
        int i = 0;

        while (i < str_len) {
                str[i] = peek();

                if (str[i] == '\\') {
                        advance();
                        i++;
                }

                advance();
                i++;
        }
        str[i] = '\0';

        char unescaped_str[end - c + 1];

        unescape_string(str, unescaped_str);

        add_token_str(STRING, unescaped_str);
}

void init(char *src) {
        code = src;
        c = 0;
        line = 1;
        head = NULL;
        end = NULL;
}

struct Token * tokenize(char *src) {
        init(src);

        char c;

        while (!at_end()) {
                c = peek();
                switch (c) {
                case '\"':
                        add_string();
                        break;
                case '=':
                        advance();
                        add_token(match('=') ? EQUAL_EQUAL : EQUAL, NULL);
                        break;
                case '!':
                        advance();
                        add_token(match('=') ? BANG_EQUAL : BANG, NULL);
                        break;
                case '-':
                        add_token(MINUS, NULL);
                        break;
                case '+':
                        add_token(PLUS, NULL);
                        break;
                case '*':
                        add_token(STAR, NULL);
                        break;
                case '/':
                        advance();
                        if (match('/')) {
                                while (!at_end() && peek() != '\n') advance();
                        } else {
                                add_token(SLASH, NULL);
                        }
                        break;
                case '>':
                        advance();
                        add_token(match('=') ? GREATER_EQUAL : GREATER, NULL);
                        break;
                case '<':
                        advance();
                        add_token(match('=') ? LESS_EQUAL : LESS, NULL);
                        break;
                case ';':
                        add_token(SEMICOLON, NULL);
                        break;
                case '(':
                        add_token(LEFT_PAREN, NULL);
                        break;
                case ')':
                        add_token(RIGHT_PAREN, NULL);
                        break;
                case '{':
                        add_token(LEFT_BRACE, NULL);
                        break;
                case '}':
                        add_token(RIGHT_BRACE, NULL);
                        break;
                case ',':
                        add_token(COMMA, NULL);
                        break;
                case '.':
                        add_token(DOT, NULL);
                        break;
                case '\n':
                        line++; /* fall through */
                case ' ':
                case '\t':
                        break;
                default:
                        if (is_num(peek())) {
                                add_number();
                                continue;
                        } else if (is_alpha(peek())) {
                                add_identifier();
                                continue;
                        } else {
                                error(line, "Unexpected character");
                                printf("%c", peek());
                        }
                }
                advance();
        }

        add_token(END, NULL);

        return head;
}
