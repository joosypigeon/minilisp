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

// ───── Main ─────
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    global = malloc(sizeof(Env));
    global->bindings = NULL;
    global->parent = NULL;
    DEBUG_PRINT_VERBOSE("global env: %p\n", global);
    char *source = read_file(argv[1]);
    tokenize(source);
    while (current_token < num_tokens) {
        Object *expr = parse_expr();
        Object *result = eval(global, expr);
        print_object(result);
        printf("\n");
    }
    free(source);
    return 0;
}
