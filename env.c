#include "env.h"
#include "debug.h"

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