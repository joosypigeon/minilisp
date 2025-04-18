#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "util.h"
#include <gmp.h>

typedef enum {
    TYPE_INT,
    TYPE_SYMBOL,
    TYPE_LAMBDA,
    TYPE_PAIR,
    TYPE_NIL
} Type;

typedef struct Object Object;

// Singleton NIL object
extern Object *NIL;

typedef struct Env Env; // forward declaration for closure support

struct Object {
    Type type;
    union {
        mpz_t int_val;
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

Object *make_number(int value);
Object *make_number_from_string(const char *tok);
Object *make_symbol(const char *name);
Object *make_lambda(Object *params, Object *body, Env *env);
Object *make_nil(void);
Object *cons(Object *car, Object *cdr);
Object *car(Object *obj);
Object *cdr(Object *obj);
Object *cddr(Object *obj);
Object *cadr(Object *obj);
Object *caddr(Object *obj);
Object *cadddr(Object *obj);
int list_length(Object *obj);
char *type_to_string(Type type);
void print_type(Type type);
void print_object(Object *obj);
char *object_to_string(Object *obj);
bool object_equal(Object *a, Object *b);
Object *lisp_eq(Object *a, Object *b);
Object *lisp_equal(Object *a, Object *b);
#endif // OBJECT_H
