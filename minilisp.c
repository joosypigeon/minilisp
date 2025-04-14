#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "debug.h"
#include "util.h"
#include "object.h"



#define MAX_TOKENS 1024
char *tokens[MAX_TOKENS];
int num_tokens = 0;
int current_token = 0;

// ───── Environment ─────
typedef struct Binding {
    char *name;
    Object *value;
    struct Binding *next;
} Binding;

typedef struct Env {
    Binding *bindings;
    struct Env *parent;
} Env;

Env *global = NULL;

void env_define(Env *local, const char *name, Object *value) {
    Binding *b = malloc(sizeof(Binding));
    b->name = xstrdup(name);
    b->value = value;
    b->next = local->bindings;
    local->bindings = b;
}

Object *env_lookup(Env *local, const char *name) {
    for (Env* level = local; level; level = level->parent) {
        for (Binding *b = level->bindings; b != NULL; b = b->next) {
            if (strcmp(b->name, name) == 0) {
                return b->value;
            }
        }
    }
    fprintf(stderr, "Unbound symbol: %s\n", name);
    exit(1);
}

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
        } else {
            const char* start = p;
            while (*p && !isspace(*p) && *p != '(' && *p != ')' && *p != ';' && *p != SYM_CHAR_QUOTE) p++;
            if (*p == SYM_CHAR_QUOTE) {
                 if(*(p+1) != '\0' && !isspace(*(p+1))) {
                    tokens[num_tokens++] = make_token(&"(", 1);
                    tokens[num_tokens++] = xstrdup("quote");
                    p++;
                    start=p;
                    while (*p && !isspace(*p) && *p != '(' && *p != ')' && *p != ';') p++;
                    tokens[num_tokens++] = make_token(start, p - start); 
                    tokens[num_tokens++] = make_token(&")", 1);
                    continue;
                 } else {
                    p++;
                 }
            }
            tokens[num_tokens++] = make_token(start, p - start);
        }
    }
}

// ───── Parser ─────
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
    if (strcmp(tok, "(") == 0) {
        return parse_list();
    } 
    current_token++;
    char *end;
    long val = strtol(tok, &end, 10);
    if (*end == '\0') return make_number(val);
    return make_symbol(tok);
}

// ───── Eval ─────
Object *eval(Env *local, Object *expr);

Env* push_env(Env *local){
    if(local == NULL){
        DEBUG_PRINT_ERROR("expected enviroment");
        exit(1);
    }
    Env* new = malloc(sizeof(Env));
    new->bindings = NULL;
    new->parent = local;
    return new;
}

Env* pop_env(Env *local){
    Env* parent = local->parent;
    Binding *b = local->bindings;
    while(b){
        Binding* next = b->next;
        free(b->name);
        free(b->value);
        free(b);
        b = next;
    }
    free(local);
    return parent;
}

Object* eval_function(Object* lambda, Object* args){
    DEBUG_PRINT_VERBOSE("enter eval_function\n");

    Object* params = lambda->lambda.params;
    Env *local = lambda->lambda.env;
    
    while (params != NIL && args != NIL) {
        Object *param = car(params);  // symbol
        Object *arg = eval(local, car(args));      // value
    
        if (param->type != TYPE_SYMBOL) {
            DEBUG_PRINT_ERROR("TYPE ERROR: expected a symbol\n"); 
            exit(1);
        }
        if (arg->type != TYPE_INT) {
            DEBUG_PRINT_ERROR("TYPE ERROR: expected an integer\n"); 
            exit(1);
        }

        DEBUG_PRINT_VERBOSE("param: %s, arg: %i\n", param->symbol, arg->int_val);
    
        env_define(local, param->symbol, arg);
    
        params = cdr(params);
        args = cdr(args);
    }

    Object* body = lambda->lambda.body;

    Object* value = eval(local, body);

    return value;
}

Object *eval(Env *local, Object *expr) {
    if (expr->type == TYPE_INT || expr->type == TYPE_LAMBDA) {
        return expr;
    } else if (expr->type == TYPE_SYMBOL) {
        return env_lookup(local, expr->symbol);
    } else if (expr->type == TYPE_PAIR) {
        Object *first = car(expr);
        if (first->type == TYPE_SYMBOL) {
            if (strcmp(first->symbol, SYM_QUOTE) == 0) {
                return cadr(expr);
            }
            if (strcmp(first->symbol, SYM_DEFINE) == 0) {
                Object *second = cadr(expr);
                if (second->type == TYPE_SYMBOL) {
                    Object *val = eval(local, caddr(expr));
                    env_define(local, second->symbol, val);
                    return val;
                }
                if(second->type==TYPE_PAIR){
                    DEBUG_PRINT_VERBOSE("found function\n");
                    Object *name = car(second);
                    if(name->type!=TYPE_SYMBOL){
                        DEBUG_PRINT_ERROR("TYPE ERROR: expected function name\n");
                        exit(1);
                    }
                    Object* parameters = cdr(second);
                    Object *body = caddr(expr);
                    if(body->type!=TYPE_PAIR){
                        DEBUG_PRINT_ERROR("TYPE ERROR: expected function body\n");
                        exit(1);
                    }
                    Env *env = push_env(local);
                    Object* lambda = make_lambda(parameters, body, env);

                    env_define(local, name->symbol, lambda);

                    return lambda;
                }
            }
            if (strcmp(first->symbol, SYM_PLUS) == 0) {
                int sum = 0;
                Object *iter = cdr(expr);
                while (iter != NIL) {
                    Object *item = car(iter);      // extract current item
                    Object *val = eval(local, item);   // evaluate
                    if (val->type != TYPE_INT) {
                        DEBUG_PRINT_ERROR("Type error: expected int\n");
                        exit(1);
                    }
                    sum += val->int_val;
                    iter = cdr(iter);              // move to next
                }

                Object* result = make_number(sum);
                return result;
            }
            if (strcmp(first->symbol, SYM_TIMES) == 0) {
                int sum = 1;
                Object *iter = cdr(expr);
                while (iter != NIL) {
                    Object *item = car(iter);      // extract current item
                    Object *val = eval(local, item);   // evaluate
                    if (val->type != TYPE_INT) {
                        DEBUG_PRINT_ERROR("Type error: expected int\n");
                        exit(1);
                    }
                    sum *= val->int_val;
                    iter = cdr(iter);              // move to next
                }

                Object* result = make_number(sum);
                return result;
            }  
            if (strcmp(first->symbol, SYM_LAMBDA) == 0) {
                Env *env = push_env(local);
                Object* params = cadr(expr);
                if(params->type!=TYPE_PAIR && params->type!=TYPE_NIL){
                    DEBUG_PRINT_ERROR("TYPE ERROR: expected parameters\n");
                    exit(1);
                }
                Object *body = caddr(expr);
                /*
                if(body->type!=TYPE_PAIR && body->type!=TYPE_INT){
                    DEBUG_PRINT_ERROR("TYPE ERROR: expected function body\n");
                    exit(1);
                }
                */
                Object* lambda = make_lambda(params, body, env);
                return lambda;
            }
            if (strcmp(first->symbol, SYM_EQUAL) == 0) {
                Object *second = eval(local, cadr(expr));
                Object *third = eval(local, caddr(expr));
                return lisp_equal(second, third);
            }
            /* expect function call*/
            Object *lambda = env_lookup(local, first->symbol);
            if(lambda->type!=TYPE_LAMBDA){
                DEBUG_PRINT_ERROR("TYPE ERROR: expected lambda expression\n");
                exit(1);
            }
            Object* arg_list = cdr(expr);
            Object* value = eval_function(lambda, arg_list);
            return value;
        }
        /* Should have a lambda expression*/
        if (first->type == TYPE_PAIR) {
            Object* lambda = eval(local, first);
            if(lambda->type!=TYPE_LAMBDA) {
                DEBUG_PRINT_ERROR("TYPE ERROR: expected lambda expresions, got %s\n", type_to_string(lambda->type));
                exit(1);               
            }
            Object *args = cdr(expr);
            Object* value = eval_function(lambda, args);
            return value;
        }


        printf("Cannot evaluate expression yet: ");
        print_object(expr);
        printf("\n");
        exit(1);
    } else {
        return expr;
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

// ───── Main ─────
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    global = malloc(sizeof(Env));
    global->bindings = NULL;
    global->parent = NULL;
    
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
