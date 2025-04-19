#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "util.h"
#include "tokenize.h"
#include "symbols.h"
#include "debug.h"

#define MAX_TOKENS 1024
char *tokens[MAX_TOKENS];
int num_tokens = 0;
int current_token = 0;



// ───── Tokenizer ─────
static void skip_whitespace_and_comments(const char **p) {
    while (isspace(**p)) (*p)++;
    if (**p == ';') while (**p && **p != '\n') (*p)++;
}

static char *make_token(const char *start, int len) {
    char *tok = malloc(len + 1);
    memcpy(tok, start, len);
    tok[len] = '\0';
    return tok;
}

void tokenize(const char* input) {
    num_tokens = 0;
    const char* p = input;

    while (*p) {
        skip_whitespace_and_comments(&p);

        if (*p == '\0') break;

        if (*p == '(' || *p == ')') {
            tokens[num_tokens++] = make_token(p++, 1);
            continue;
        }

        if (*p == '"') {
            const char* start = p;
            p++;  // skip opening quote
            while (*p && *p != '"') p++;
            if (*p == '\0') {
                DEBUG_PRINT_ERROR("tokenize: unterminated string\n");
                exit(1);
            }
            int len = p - start + 1; // include closing quote
            tokens[num_tokens++] = make_token(start, len);
            p++;  // skip closing quote
            continue;
        }

        const char* start = p;
        while (*p && !isspace(*p) && *p != '(' && *p != ')' && *p != ';' && *p != SYM_CHAR_QUOTE) p++;
        if (*p == SYM_CHAR_QUOTE) {
                if(*(p+1) != '\0' && !isspace(*(p+1))) {
                tokens[num_tokens++] = make_token("(", 1);
                tokens[num_tokens++] = xstrdup("quote");
                p++;
                start=p;
                while (*p && !isspace(*p) && *p != '(' && *p != ')' && *p != ';') p++;
                tokens[num_tokens++] = make_token(start, p - start); 
                tokens[num_tokens++] = make_token(")", 1);
                continue;
                } else {
                p++;
                }
        }
        int len = p - start;
        if (len == 0) continue; // empty token
        tokens[num_tokens++] = make_token(start, len);
}
}

// ───── File Reader ─────
char *read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(len + 1);
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}