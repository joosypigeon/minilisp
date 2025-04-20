#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "debug.h"
#include "util.h"
#include "object.h"
#include "env.h"
#include "tokenize.h"
#include "symbols.h"
#include "parse.h"
#include "eval.h"


#define BUFFER_SIZE 1024

void repl() {
    char line[BUFFER_SIZE];
    printf("MiniLisp REPL (type (exit) to quit)\n");

    while (1) {
        printf("minilisp> ");
        if (!fgets(line, BUFFER_SIZE, stdin)) break;

        if (strcmp(line, "\n") == 0) continue;
        if (strncmp(line, "(exit)", 6) == 0) break;

        tokenize(line);
        current_token = 0;

        while (current_token < num_tokens) {
            Object *expr = parse_expr();
            Object *result = eval(global, expr);
            print_object(result);
            printf("\n");
        }
    }

    printf("Goodbye.\n");
}

void batch (char *filename) {
    char *source = read_file(filename);
    tokenize(source);
    current_token = 0;

    while (current_token < num_tokens) {
        Object *expr = parse_expr();
        Object *result = eval(global, expr);
        print_object(result);
        printf("\n");
    }
    free(source);
}

// ───── Main ─────
int main(int argc, char **argv) {
    DEBUG_PRINT_VERBOSE("main: argc: %d, argv[1]: %s\n", argc, argv[1]);
    
    global = malloc(sizeof(Env));
    global->bindings = NULL;
    global->parent = NULL;
    DEBUG_PRINT_VERBOSE("global env: %p\n", global);

    if (argc == 2) {
        batch(argv[1]);
        free(global);
        return 0;
    }

    repl();
    free(global);
    return 0;
}
