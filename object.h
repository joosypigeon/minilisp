#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "util.h"

#define SYM_INT     "int"
#define SYM_SYMBOL  "symbol"
#define SYM_LAMBDA  "lambda"
#define SYM_PAIR    "pair"
#define SYM_NIL     "nil"
#define SYM_UNKOWN  "unkown"

#define SYM_DEFINE  "define"
#define SYM_QUOTE   "quote"
#define SYM_EQUAL   "equal?"
#define SYM_PLUS    "+"
#define SYM_TIMES   "*"
#define SYM_CHAR_QUOTE  '\''
#define SYM_SPACE  ' '

typedef enum {
    TYPE_INT,
    TYPE_SYMBOL,
    TYPE_LAMBDA,
    TYPE_PAIR,
    TYPE_NIL
} Type;

typedef struct Object Object;

typedef struct Env Env; // forward declaration for closure support

struct Object {
    Type type;
    union {
        int int_val;
        char *symbol;

        struct {
            Object *params;
            Object *body;
            Env *env;
        } lambda;

        struct {
            Object *car;
            Object *cdr;
        }; // for TYPE_PAIR
    };
};

// Singleton NIL object
static Object *NIL = &(Object){ .type = TYPE_NIL };

// Singleton true object
static Object *TRUE = &(Object){ .type = TYPE_SYMBOL, .symbol = "#t" };

bool islist(Object *item){
    return item->type==TYPE_PAIR;
}

Object *make_number(int n) {
    Object *obj = malloc(sizeof(Object));
    obj->type = TYPE_INT;
    obj->int_val = n;
    return obj;
}

Object *make_symbol(const char *name) {
    Object *obj = malloc(sizeof(Object));
    obj->type = TYPE_SYMBOL;
    obj->symbol = xstrdup(name);
    return obj;
}

Object *make_lambda(Object *params, Object *body, Env *env) {
    Object *obj = malloc(sizeof(Object));
    obj->type = TYPE_LAMBDA;
    obj->lambda.params = params;
    obj->lambda.body = body;
    obj->lambda.env = env;
    return obj;
}

Object *make_nil(void) {
    return NIL;
}

Object *cons(Object *car, Object *cdr) {
    Object *obj = malloc(sizeof(Object));
    obj->type = TYPE_PAIR;
    obj->car = car;
    obj->cdr = cdr;
    return obj;
}

Object *car(Object *obj) {
    if (obj->type != TYPE_PAIR) {
        fprintf(stderr, "Error: car called on non-pair\n");
        exit(1);
    }
    return obj->car;
}

Object *cdr(Object *obj) {
    if (obj->type != TYPE_PAIR) {
        fprintf(stderr, "Error: cdr called on non-pair\n");
        exit(1);
    }
    return obj->cdr;
}

Object *cadr(Object *obj) {
    return car(cdr(obj));
}

Object *caddr(Object *obj) { return car(cdr(cdr(obj))); }

char * type_to_string(Type type) {
    char *s = NULL;
    switch (type) {
        case TYPE_INT:    s = xstrdup(SYM_INT);
        case TYPE_SYMBOL: s = xstrdup(SYM_SYMBOL);
        case TYPE_LAMBDA: s = xstrdup(SYM_LAMBDA);
        case TYPE_PAIR:   s = xstrdup(SYM_PAIR);
        case TYPE_NIL:    s = xstrdup(SYM_NIL);
        default:          s = xstrdup(SYM_UNKOWN);
    }
    return s;
}

void print_type(Type type) {
    printf("%s", type_to_string(type));
}

void print_object(Object *obj) {
    switch (obj->type) {
        case TYPE_INT:
            printf("%d", obj->int_val);
            break;
        case TYPE_SYMBOL:
            printf("%s", obj->symbol);
            break;
        case TYPE_LAMBDA:
            printf("(lambda ");
            print_object(obj->lambda.params);
            printf(" ");
            print_object(obj->lambda.body);
            printf(")");
            break;
        case TYPE_PAIR:
            printf("(");
            while (obj->type == TYPE_PAIR) {
                print_object(obj->car);
                obj = obj->cdr;
                if (obj->type == TYPE_PAIR) printf(" ");
            }
            if (obj != NIL) {
                printf(" . ");
                print_object(obj);
            }
            printf(")");
            break;
        case TYPE_NIL:
            printf("()");
            break;
        default:
            printf("<unknown>");
            break;
    }
}

bool object_equal(Object *a, Object *b) {
    if (a == b) return true;

    if (a->type != b->type) return false;

    switch (a->type) {
        case TYPE_INT:
            return a->int_val == b->int_val;

        case TYPE_SYMBOL:
            return strcmp(a->symbol, b->symbol) == 0;

        case TYPE_NIL:
            return true;

        case TYPE_PAIR:
            return object_equal(a->car, b->car) &&
                   object_equal(a->cdr, b->cdr);

        default:
            return false;
    }
}

Object *lisp_eq(Object *a, Object *b) {
    return (a == b) ? TRUE : NIL;
}

Object *lisp_equal(Object *a, Object *b) {
    return object_equal(a, b) ? TRUE : NIL;
}

#endif // OBJECT_H
