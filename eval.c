#include "eval.h"
#include "debug.h"
#include "symbols.h"



typedef Object *(*SpecialFormHandler)(Env *, Object *);

Object *handle_quote(Env *env, Object *expr) {
    int length = list_length(expr);
    if (length != 2) {
        DEBUG_PRINT_ERROR("Invalid quote form\n");
        exit(1);
    }
    return cadr(expr);
}

Object *handle_define(Env *local, Object *expr) {
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
            DEBUG_PRINT_ERROR("expected function name\n");
            exit(1);
        }
        Object* parameters = cdr(second);
        Object *body = caddr(expr);
        if(body->type!=TYPE_PAIR){
            DEBUG_PRINT_ERROR("expected function body\n");
            exit(1);
        }
        Env *env = push_env(local);
        Object* lambda = make_lambda(parameters, body, env);

        env_define(local, name->symbol, lambda);

        return lambda;
    }

    DEBUG_PRINT_ERROR("Invalid define form\n");
    exit(1);
}

Object *handle_plus(Env *local, Object *expr) {
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

Object *handle_times(Env *local, Object *expr) {
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
    Object *second = eval(local, cadr(expr));
    Object *third = eval(local, caddr(expr));
    return lisp_equal(second, third);
}

typedef struct {
    const char *symbol;
    SpecialFormHandler handler;
} DispatchEntry;

DispatchEntry special_forms[] = {
    { SYM_QUOTE, handle_quote },
    { SYM_DEFINE, handle_define },
    { SYM_PLUS, handle_plus},
    { SYM_TIMES, handle_times},
    { SYM_LAMBDA, handle_lambda},
    { SYM_EQUAL, handle_equal},
    // ... add more here
    { NULL, NULL }
};




Object *eval(Env *local, Object *expr);

Object *eval_function(Object* lambda, Object* args){
    DEBUG_PRINT_VERBOSE("enter eval_function\n");

    Object* params = lambda->lambda.params;
    Env *local = lambda->lambda.env;

    int params_length = list_length(params);
    int args_length = list_length(args);
    if (params_length != args_length) {
        DEBUG_PRINT_ERROR("TYPE ERROR: expected %i arguments, got %i\n", params_length, args_length);
        exit(1);
    }
    
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

Object *eval_list(Env *local, Object *expr) {
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

    DEBUG_PRINT_ERROR("Cannot evaluate expression yet\n");
    print_object(expr);
    printf("\n");
    exit(1);
}

Object *eval(Env *local, Object *expr) {
    if (expr->type == TYPE_INT || expr->type == TYPE_LAMBDA) {
        return expr;
    } else if (expr->type == TYPE_SYMBOL) {
        return env_lookup(local, expr->symbol);
    } else if (expr->type == TYPE_PAIR) {
        return eval_list(local, expr);
    } else {
        return expr;
    }
}