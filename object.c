#include "object.h"
#include "debug.h"
#include "symbols.h"

// Singleton NIL object
Object *NIL = &(Object){ .type = TYPE_NIL };

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

int list_length(Object *obj) {
    if (obj == NIL) return 0;

    int len = 0;

    while (obj != NIL) {
        if (!obj) {
            DEBUG_PRINT_ERROR("unexpected NULL object in list\n");
            exit(1);
        }

        if (obj->type != TYPE_PAIR) {
            DEBUG_PRINT_ERROR("list_length: not TYPE_PAIR, found %s\n",
                              type_to_string(obj->type));
            exit(1);
        }

        len++;
        obj = obj->cdr;
    }

    return len;
}


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
    char tmp[64];

    switch (obj->type) {
        case TYPE_INT:
            snprintf(tmp, sizeof(tmp), "%d", obj->int_val);
            append_str(buf, size, used, tmp);
            break;

        case TYPE_SYMBOL:
            append_str(buf, size, used, obj->symbol);
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
                if (obj->type == TYPE_PAIR) {
                    append_char(buf, size, used, ' ');
                }
            }
            if (obj != NIL) {
                append_str(buf, size, used, " . ");
                object_to_string_internal(obj, buf, size, used);
            }
            append_char(buf, size, used, ')');
            break;

        case TYPE_NIL:
            append_str(buf, size, used, "()");
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