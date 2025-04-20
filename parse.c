#include "parse.h"
#include "tokenize.h"
#include "symbols.h"
#include "debug.h"


Object *parse_expr();

Object *parse_list() {
    current_token++;  // skip the opening '('

    Object *head = NIL;
    Object *tail = NIL;

    while (current_token < num_tokens && strcmp(tokens[current_token], ")") != 0) {
        Object *item = parse_expr();
        Object *node = cons(item, NIL);

        if (head == NIL) {
            head = node;
            tail = node;
        } else {
            tail->cdr = node;
            tail = node;
        }
    }

    current_token++;  // skip the closing ')'
    return head;
}


Object *parse_expr() {
    char *tok = tokens[current_token];

    if (strcmp(tok, "'") == 0) {
        current_token++;  // skip the quote token
        Object *quoted = parse_expr();  // parse the thing being quoted
        return cons(make_symbol("quote"), cons(quoted, NIL));
    }

    if (strcmp(tok, "(") == 0) {
        return parse_list();
    } 
    current_token++;

    if (tok[0] == '"' && tok[strlen(tok) - 1] == '"') {
        tok[strlen(tok) - 1] = '\0';  // remove trailing "
        return make_string(tok + 1);  // skip initial "
    }

    char *end;
    long val = strtol(tok, &end, 10);
    if (*end == '\0') return make_number_from_string(tok);
    if (strcmp(tok, SYM_FALSE) == 0) return make_nil();
    if (strcmp(tok, SYM_NIL) == 0) return make_nil();
    if (strcmp(tok, SYM_TRUE) == 0) return make_true();
    return make_symbol(tok);
}