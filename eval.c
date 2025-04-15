#include "eval.h"
#include "debug.h"
#include "symbols.h"

Object *eval(Env *local, Object *expr);

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