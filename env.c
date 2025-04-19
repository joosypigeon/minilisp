#include "env.h"
#include "debug.h"
#include "gmp.h"

Env *global = NULL;

void env_define(Env *local, const char *name, Object *value) {
    Binding *b = malloc(sizeof(Binding));
    b->name = xstrdup(name);
    b->value = value;
    b->next = local->bindings;
    local->bindings = b;
    DEBUG_PRINT_VERBOSE("env_define: env: %p, %s = %s\n", local, name, object_to_string(value));
    if (mpz_cmp_ui(value->int_val, 0) < 0) {
        DEBUG_PRINT_ERROR("Invalid number: %s\n", object_to_string(value));
        exit(1);
    }
}

bool set(Env *local, const char *name, Object *value) {
    DEBUG_PRINT_VERBOSE("set: env: %p, %s = %s\n", local, name, object_to_string(value));
    for (Env* level = local; level; level = level->parent) {
        for (Binding *b = level->bindings; b != NULL; b = b->next) {
            if (strcmp(b->name, name) == 0) {
                b->value = value;
                return true;
            }
        }
    }
    return false;
}

Object *env_lookup(Env *local, const char *name) {
    DEBUG_PRINT_VERBOSE("enter: env_lookup: env: %p, %s\n", local, name);
    for (Env* level = local; level; level = level->parent) {
        DEBUG_PRINT_VERBOSE("env_lookup: level: %p\n", level);
        for (Binding *b = level->bindings; b != NULL; b = b->next) {
            if (strcmp(b->name, name) == 0) {
                DEBUG_PRINT_VERBOSE("env_lookup: env: %p, %s = %s\n", level, name, object_to_string(b->value));
                return b->value;
            }
        }
    }
    DEBUG_PRINT_ERROR("env_lookup: env: %p, %s not found\n", local, name);
    exit(1);
}

Env* push_env(Env *local){
    if(local == NULL){
        DEBUG_PRINT_ERROR("expected enviroment\n");
        exit(1);
    }
    Env* new = malloc(sizeof(Env));
    new->bindings = NULL;
    new->parent = local;
    DEBUG_PRINT_VERBOSE("push_env: new env: %p, parent: %p\n", new, local);
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