#include "env.h"
#include "debug.h"

Env *global = NULL;

void env_define(Env *local, const char *name, Object *value) {
    Binding *b = malloc(sizeof(Binding));
    b->name = xstrdup(name);
    b->value = value;
    b->next = local->bindings;
    local->bindings = b;
    DEBUG_PRINT_VERBOSE("env_define: env: %p, %s = %s\n", local, name, object_to_string(value));
}

Object *env_lookup(Env *local, const char *name) {
    for (Env* level = local; level; level = level->parent) {
        for (Binding *b = level->bindings; b != NULL; b = b->next) {
            if (strcmp(b->name, name) == 0) {
                DEBUG_PRINT_VERBOSE("env_lookup: env: %p, %s = %s\n", level, name, object_to_string(b->value));
                return b->value;
            }
        }
    }
    fprintf(stderr, "Unbound symbol: %s\n", name);
    exit(1);
}

Env* push_env(Env *local){
    if(local == NULL){
        DEBUG_PRINT_ERROR("expected enviroment");
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