#include "eval.h"
#include "debug.h"
#include "symbols.h"



typedef Object *(*SpecialFormHandler)(Env *, Object *);

Object *handle_quote(Env *env, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: handle_quote: env: %p, expr: %s\n", env, object_to_string(expr));
    int length = list_length(expr);
    if (length != 2) {
        DEBUG_PRINT_ERROR("Invalid quote form\n");
        exit(1);
    }
    return cadr(expr);
}

Object *handle_define(Env *local, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: handle_define: local: %p, expr: %s\n", local, object_to_string(expr));
    int length = list_length(expr);
    if (length != 3) {
        DEBUG_PRINT_ERROR("Invalid define form\n");
        exit(1);
    }
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
            DEBUG_PRINT_ERROR("handle_define: expected function name\n");
            exit(1);
        }
        Object* parameters = cdr(second);
        Object *body = caddr(expr);
        if(body->type!=TYPE_PAIR){
            DEBUG_PRINT_ERROR("handle_define: expected function body\n");
            exit(1);
        }

        Object* lambda = make_lambda(parameters, body, local);

        env_define(local, name->symbol, lambda);

        return lambda;
    }

    DEBUG_PRINT_ERROR("Invalid define form\n");
    exit(1);
}

Object *handle_set(Env *local, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: handle_set: local: %p, expr: %s\n", local, object_to_string(expr));
    int length = list_length(expr);
    if (length != 3) {
        DEBUG_PRINT_ERROR("Invalid define form\n");
        exit(1);
    }
    Object *second = cadr(expr);
    if (second->type == TYPE_SYMBOL) {
        Object *val = eval(local, caddr(expr));
        bool found = set(local, second->symbol, val);
        if (!found) {
            DEBUG_PRINT_ERROR("handle_set: %s not found\n", second->symbol);
            exit(1);
        }
        return val;
    }
    if(second->type==TYPE_PAIR){
        DEBUG_PRINT_VERBOSE("found function\n");
        Object *name = car(second);
        if(name->type!=TYPE_SYMBOL){
            DEBUG_PRINT_ERROR("handle_set: expected function name\n");
            exit(1);
        }
        Object* parameters = cdr(second);
        Object *body = caddr(expr);
        if(body->type!=TYPE_PAIR){
            DEBUG_PRINT_ERROR("handle_set: expected function body\n");
            exit(1);
        }
        Env *env = push_env(local);
        Object* lambda = make_lambda(parameters, body, env);

        bool found = set(local, second->symbol, lambda);
        if (!found) {
            DEBUG_PRINT_ERROR("handle_set: %s not found\n", second->symbol);
            exit(1);
        }

        return lambda;
    }

    DEBUG_PRINT_ERROR("Invalid define form\n");
    exit(1);
}

Object *handle_plus(Env *env, Object *expr) {
    Object *iter = cdr(expr);
    Object *result = make_number(0); // start with 0

    while (iter != NIL) {
        Object *val = eval(env, car(iter));
        if (val->type != TYPE_INT) {
            DEBUG_PRINT_ERROR("+: expected int\n");
            exit(1);
        }
        mpz_add(result->int_val, result->int_val, val->int_val);
        iter = cdr(iter);
    }

    return result;
}

Object *handle_minus(Env *local, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: handle_minus: local: %p, expr: %s\n", local, object_to_string(expr));
    
    int length = list_length(expr);
    if (length < 2) {
        DEBUG_PRINT_ERROR("Invalid minus form %s\n", object_to_string(expr));
        exit(1);
    }

    Object *first = eval(local, cadr(expr));
    if (first->type != TYPE_INT) {
        DEBUG_PRINT_ERROR("Type error: expected int, found %s\n", object_to_string(first));
        exit(1);
    }

    Object *result = make_number(0);  // initialize result

    if (length == 2) {
        // Unary negation: (- x)
        mpz_neg(result->int_val, first->int_val);
        return result;
    }

    // Start with the first operand
    mpz_set(result->int_val, first->int_val);

    // Iterate through the rest and subtract each
    Object *iter = cddr(expr);
    while (iter != NIL) {
        Object *item = eval(local, car(iter));
        if (item->type != TYPE_INT) {
            DEBUG_PRINT_ERROR("Type error: expected int, got %s\n", object_to_string(item));
            exit(1);
        }

        mpz_sub(result->int_val, result->int_val, item->int_val);
        iter = cdr(iter);
    }

    return result;
}

Object *handle_times(Env *local, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: handle_times: local: %p, expr: %s\n", local, object_to_string(expr));

    Object *result = make_number(1);  // initialize to 1

    Object *iter = cdr(expr);
    while (iter != NIL) {
        Object *item = eval(local, car(iter));   // evaluate
        if (item->type != TYPE_INT) {
            DEBUG_PRINT_ERROR("Type error: expected int, got %s\n", object_to_string(item));
            exit(1);
        }

        mpz_mul(result->int_val, result->int_val, item->int_val);  // result *= item
        iter = cdr(iter);
    }

    return result;
}

Object *handle_lambda(Env *local, Object *expr) {
    Env *env = push_env(local);
    Object* params = cadr(expr);
    if(params->type!=TYPE_PAIR && params->type!=TYPE_NIL){
        DEBUG_PRINT_ERROR("expected parameters\n");
        exit(1);
    }
    Object *body = caddr(expr);
    Object* lambda = make_lambda(params, body, env);
    return lambda;
}

Object *handle_equal(Env *local, Object *expr){
    DEBUG_PRINT_VERBOSE("enter: handle_equal: local: %p, expr: %s\n", local, object_to_string(expr));
    Object *second = eval(local, cadr(expr));
    Object *third = eval(local, caddr(expr));
    return lisp_equal(second, third);
}

Object *handle_if(Env *env, Object *expr) {
    int length = list_length(expr);
    if (length != 4) {
        DEBUG_PRINT_ERROR("if: wrong number of arguments, expected 3\n");
        exit(1);
    }

    Object *condition = cadr(expr);
    Object *then_branch = caddr(expr);
    Object *else_branch = cadddr(expr);

    Object *cond_result = eval(env, condition);
    if (cond_result != NIL) {
        return eval(env, then_branch);
    } else {
        return eval(env, else_branch);
    }
}

Object *handle_cons(Env *env, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: handle_cons: env: %p, expr: %s\n", env, object_to_string(expr));
    int length = list_length(expr);
    if (length != 3) {
        DEBUG_PRINT_ERROR("Invalid cons form\n");
        exit(1);
    }
    Object *first = eval(env, cadr(expr));
    Object *second = eval(env, caddr(expr));
    Object *result = cons(first, second);
    return result;
}

Object *handle_car(Env *env, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: handle_car: env: %p, expr: %s\n", env, object_to_string(expr));
    int length = list_length(expr);
    if (length != 2) {
        DEBUG_PRINT_ERROR("Invalid car form\n");
        exit(1);
    }
    Object *first = eval(env, cadr(expr));

    if (first->type != TYPE_PAIR) {
        DEBUG_PRINT_ERROR("car: expected pair\n");
        exit(1);
    }
    Object *result = first->car;
    return result;
}

Object *handle_cdr(Env *env, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: handle_cdr: env: %p, expr: %s\n", env, object_to_string(expr));
    int length = list_length(expr);
    if (length != 2) {
        DEBUG_PRINT_ERROR("Invalid cdr form\n");
        exit(1);
    }
    Object *first = eval(env, cadr(expr));
    if (first->type != TYPE_PAIR) {
        DEBUG_PRINT_ERROR("cdr: expected pair\n");
        exit(1);
    }
    Object *result = first->cdr;
    return result;
}

Object *handle_begin(Env *env, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: begin: env: %p, expr = %s\n", env, object_to_string(expr));

    Object *body = cdr(expr);  // skip the symbol "begin"
    Object *result = NIL;

    while (body != NIL) {
        result = eval(env, car(body));
        body = cdr(body);
    }
    DEBUG_PRINT_VERBOSE("leave: begin: result = %s\n", object_to_string(result));
    return result;  // return the last result
}

Object *handle_let(Env *env, Object *expr) {
    // (let ((x 1) (y 2)) body)
    Object *bindings = cadr(expr);
    Object *body = caddr(expr);

    Env *extended = push_env(env);

    while (bindings != NIL) {
        Object *pair = car(bindings);      // (x 1)
        Object *name = car(pair);          // x
        Object *value_expr = cadr(pair);   // 1
        Object *value = eval(env, value_expr);  // evaluate in original env
        env_define(extended, name->symbol, value);
        bindings = cdr(bindings);
    }

    return eval(extended, body);
}

Object *handle_list(Env *env, Object *expr) {
    Object *args = cdr(expr);  // skip the "list" symbol
    Object *result = NIL;
    Object **tail = &result;

    while (args != NIL) {
        Object *item = eval(env, car(args));
        Object *node = cons(item, NIL);
        *tail = node;
        tail = &node->cdr;
        args = cdr(args);
    }

    return result;
}

Object *handle_print(Env *env, Object *expr) {
    int length = list_length(expr);
    if (length != 2) {
        DEBUG_PRINT_ERROR("print: expected 1 argument, got %d\n", length - 1);
        exit(1);
    }

    Object *value = eval(env, cadr(expr));
    print_object(value);
    printf("\n");  // newline after print
    return value;  // return the printed value (common in Lisp)
}

typedef struct {
    const char *symbol;
    SpecialFormHandler handler;
} DispatchEntry;

DispatchEntry special_forms[] = {
    { SYM_QUOTE, handle_quote },
    { SYM_DEFINE, handle_define },
    { SYM_PLUS, handle_plus},
    { SYM_MINUS, handle_minus},
    { SYM_TIMES, handle_times},
    { SYM_LAMBDA, handle_lambda},
    { SYM_EQUAL, handle_equal},
    { SYM_IF, handle_if},
    { SYM_SET, handle_set},
    { SYM_CONS, handle_cons},
    { SYM_CAR, handle_car},
    { SYM_CDR, handle_cdr},
    { SYM_BEGIN, handle_begin},
    { SYM_LET, handle_let},
    { SYM_PRINT, handle_print},
    { SYM_LIST, handle_list},
    // ... add more here
    { NULL, NULL }
};

Object *eval(Env *local, Object *expr);

Object *eval_function(Env *env, Object* lambda, Object* args){
    DEBUG_PRINT_VERBOSE("enter: eval_function: lambda: %s, args: %s\n", object_to_string(lambda), object_to_string(args));

    Object* params = lambda->lambda.params;

    //Env *local = push_env(env);
    Env *local = push_env(lambda->lambda.env);

    int params_length = list_length(params);
    int args_length = list_length(args);
    if (params_length != args_length) {
        DEBUG_PRINT_ERROR("TYPE ERROR: expected %i arguments, got %i\n", params_length, args_length);
        exit(1);
    }
    
    while (params != NIL && args != NIL) {
        Object *param = car(params);  // symbol
        Object *arg = eval(env, car(args));      // value
    
        if (param->type != TYPE_SYMBOL) {
            DEBUG_PRINT_ERROR("TYPE ERROR: expected a symbol\n"); 
            exit(1);
        }
    
        DEBUG_PRINT_VERBOSE("eval_function: param: %s, arg: %s\n", param->symbol, object_to_string(arg));
    
        env_define(local, param->symbol, arg);
    
        params = cdr(params);
        args = cdr(args);
    }

    Object* body = lambda->lambda.body;

    Object* value = eval(local, body);
    DEBUG_PRINT_VERBOSE("leave: eval_function: value: %s\n", object_to_string(value));
    return value;
}

Object *eval_list(Env *local, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: eval_list: local: %p, expr: %s\n", local, object_to_string(expr));
    Object *first = car(expr);
    if (first->type == TYPE_SYMBOL) {
        for (int i = 0; special_forms[i].symbol != NULL; i++) {
            if (strcmp(first->symbol, special_forms[i].symbol) == 0) {
                return special_forms[i].handler(local, expr);
            }
        }
        /* expect function call*/
        Object *lambda = env_lookup(local, first->symbol);
        if(lambda->type!=TYPE_LAMBDA){
            DEBUG_PRINT_ERROR("TYPE ERROR: expected lambda expression\n");
            exit(1);
        }
        DEBUG_PRINT_VERBOSE("eval_list: found lambda object\n");
        Object* arg_list = cdr(expr);
        Object* value = eval_function(local, lambda, arg_list);
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
        Object* value = eval_function(local, lambda, args);
        return value;
    }

    DEBUG_PRINT_ERROR("Cannot evaluate expression yet\n");
    print_object(expr);
    printf("\n");
    exit(1);
}

Object *eval(Env *local, Object *expr) {
    DEBUG_PRINT_VERBOSE("enter: eval: local: %p, expr: %s\n", local, object_to_string(expr));
    Object *result = expr;
    if (expr->type == TYPE_SYMBOL)
        result = env_lookup(local, expr->symbol);
    if (expr->type == TYPE_PAIR)
        result = eval_list(local, expr);
    DEBUG_PRINT_VERBOSE("leave: eval: expr: %s\n", object_to_string(result));
    return result;
}