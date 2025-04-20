#include <stdarg.h>

#include "object.h"
#include "debug.h"
#include "symbols.h"
#include "error.h"

// Singleton NIL object
Object *NIL = &(Object){ .type = TYPE_NIL, .symbol = "#f" };

// Singleton true object
Object *TRUE = &(Object){ .type = TYPE_SYMBOL, .symbol = "#t" };

bool is_pair(Object *item){
    return item->type==TYPE_PAIR;
}

bool is_nil(Object *item){
    return item==NIL;
}

Object *make_number(int n) {
    Object *obj = malloc(sizeof(Object));
    obj->type = TYPE_INT;
    mpz_init_set_si(obj->int_val, n); // initialize and set value
    return obj;
}

Object *make_number_from_string(const char *tok) {
    Object *obj = malloc(sizeof(Object));
    obj->type = TYPE_INT;
    mpz_init_set_str(obj->int_val, tok, 10);
    if (mpz_cmp_ui(obj->int_val, 0) < 0) {
        RAISE_ERROR("Invalid number: %s\n", tok);
        exit(1);
    }
    return obj;
}

Object *make_symbol(const char *name) {
    Object *obj = malloc(sizeof(Object));
    obj->type = TYPE_SYMBOL;
    obj->symbol = xstrdup(name);
    return obj;
}

Object *make_string(const char *value) {
    Object *obj = malloc(sizeof(Object));
    obj->type = TYPE_STRING;
    obj->str_val = xstrdup(value);  // use your safe strdup
    return obj;
}

Object *make_lambda(Object *params, Object *body, Env *env) {
    DEBUG_PRINT_VERBOSE("make_lambda: env: %p, params: %s, body: %s\n",
        env, object_to_string(params), object_to_string(body));
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

Object *make_true(void) {
    return TRUE;
}

Object *make_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char *msg = malloc(256);
    vsnprintf(msg, 256, fmt, args);  // format like printf

    va_end(args);

    Object *err = malloc(sizeof(Object));
    err->type = TYPE_ERROR;
    err->error_msg = msg;

    return err;
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
        RAISE_ERROR("Error: car called on non-pair %s\n", object_to_string(obj));
        exit(1);
    }
    return obj->car;
}

Object *cdr(Object *obj) {
    if (obj->type != TYPE_PAIR) {
        RAISE_ERROR("Error: cdr called on non-pair\n");
        exit(1);
    }
    return obj->cdr;
}

Object *cadr(Object *obj) {
    return car(cdr(obj));
}

Object *cddr(Object *obj) {
    return cdr(cdr(obj));
}

Object *caddr(Object *obj) {
    return car(cdr(cdr(obj)));
}

Object *cadddr(Object *obj) {
    return car(cdr(cdr(cdr(obj))));
}

int list_length(Object *obj) {
    if (obj == NIL) return 0;

    int len = 0;

    while (obj != NIL) {
        if (!obj) {
            RAISE_ERROR("unexpected NULL object in list\n");
            exit(1);
        }

        if (obj->type != TYPE_PAIR) {
            RAISE_ERROR("list_length: not TYPE_PAIR, found %s\n",
                              type_to_string(obj->type));
            exit(1);
        }

        len++;
        obj = obj->cdr;
    }

    return len;
}


char * type_to_string(Type type) {
    switch (type) {
        case TYPE_INT:    return xstrdup(SYM_INT);
        case TYPE_SYMBOL: return xstrdup(SYM_SYMBOL);
        case TYPE_LAMBDA: return xstrdup(SYM_LAMBDA);
        case TYPE_PAIR:   return xstrdup(SYM_PAIR);
        case TYPE_NIL:    return xstrdup(SYM_NIL);
        default:          return xstrdup(SYM_UNKOWN);
    }
}

void print_type(Type type) {
    printf("%s", type_to_string(type));
}

void print_object(Object *obj) {
    char *str = object_to_string(obj);
    printf("%s", str);
    free(str);
}

#define INITIAL_BUF_SIZE 128

static void append_str(char **buf, size_t *size, size_t *used, const char *str) {
    size_t len = strlen(str);
    while (*used + len + 1 > *size) {
        *size *= 2;
        *buf = realloc(*buf, *size);
    }
    strcpy(*buf + *used, str);
    *used += len;
}

static void append_char(char **buf, size_t *size, size_t *used, char c) {
    if (*used + 2 > *size) {
        *size *= 2;
        *buf = realloc(*buf, *size);
    }
    (*buf)[(*used)++] = c;
    (*buf)[*used] = '\0';
}

static void object_to_string_internal(Object *obj, char **buf, size_t *size, size_t *used) {
    switch (obj->type) {
        case TYPE_INT: {
            char *str = mpz_get_str(NULL, 10, obj->int_val);  // base 10 string
            append_str(buf, size, used, str);
            free(str);  // important: GMP allocates it, you must free it
            break;
        }

        case TYPE_SYMBOL:
            append_str(buf, size, used, obj->symbol);
            break;

        case TYPE_STRING:
            append_char(buf, size, used, '"');
            append_str(buf, size, used, obj->str_val);
            append_char(buf, size, used, '"');
            break;

        case TYPE_LAMBDA:
            append_str(buf, size, used, "(lambda ");
            object_to_string_internal(obj->lambda.params, buf, size, used);
            append_char(buf, size, used, ' ');
            object_to_string_internal(obj->lambda.body, buf, size, used);
            append_char(buf, size, used, ')');
            break;

        case TYPE_PAIR:
            append_char(buf, size, used, '(');
            while (obj->type == TYPE_PAIR) {
                object_to_string_internal(obj->car, buf, size, used);
                obj = obj->cdr;
                if (obj->type == TYPE_PAIR)
                    append_char(buf, size, used, ' ');
            }
            if (obj != NIL) {
                append_str(buf, size, used, " . ");
                object_to_string_internal(obj, buf, size, used);
            }
            append_char(buf, size, used, ')');
            break;

        case TYPE_NIL:
            append_str(buf, size, used, "NIL");
            break;

        default:
            append_str(buf, size, used, "<unknown>");
            break;
    }
}

char *object_to_string(Object *obj) {
    size_t size = INITIAL_BUF_SIZE;
    size_t used = 0;
    char *buf = malloc(size);
    buf[0] = '\0';

    object_to_string_internal(obj, &buf, &size, &used);

    return buf;
}

bool object_equal(Object *a, Object *b) {
    DEBUG_PRINT_VERBOSE("object_equal: a: %s, b: %s\n",
        object_to_string(a), object_to_string(b));
    if (a == b) return true;

    if (a->type != b->type) return false;

    switch (a->type) {
        case TYPE_INT:
        if (mpz_cmp(a->int_val, b->int_val) == 0) {
            DEBUG_PRINT_VERBOSE("object_equal: int equal\n");
        }
        return mpz_cmp(a->int_val, b->int_val) == 0;

        case TYPE_SYMBOL:
            return strcmp(a->symbol, b->symbol) == 0;

        case TYPE_STRING:
            return strcmp(a->str_val, b->str_val) == 0;

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
    DEBUG_PRINT_VERBOSE("lisp_eq: a: %s, b: %s\n",
        object_to_string(a), object_to_string(b));
    return (a == b) ? TRUE : NIL;
}

Object *lisp_equal(Object *a, Object *b) {
    DEBUG_PRINT_VERBOSE("lisp_equal: a: %s, b: %s\n",
        object_to_string(a), object_to_string(b));
    return object_equal(a, b) ? TRUE : NIL;
}