#include "env.h"
#include "debug.h"
#include "gmp.h"
#include "error.h"
#include "gc.h"

Env *global = NULL;

void env_define(Env *local, Object *name, Object *value) {
    Binding *b = malloc(sizeof(Binding));
    b->name = name;
    b->value = value;
    b->next = local->bindings;
    local->bindings = b;
    DEBUG_PRINT_VERBOSE("env_define: env: %p, %s = %s, pointer to value: %p\n", local, object_to_string(name), object_to_string(value), value);
}

bool set(Env *local, Object *name, Object *value) {
    DEBUG_PRINT_VERBOSE("set: env: %p, %s = %s\n", local, name->symbol, object_to_string(value));
    for (Env* level = local; level; level = level->parent) {
        for (Binding *b = level->bindings; b != NULL; b = b->next) {
            if (b->name == name) {
                b->value = value;
                return true;
            }
        }
    }
    return false;
}

Object *env_lookup(Env *local, Object *name) {
    DEBUG_PRINT_VERBOSE("enter: env_lookup: env: %p, %s\n", local, name->symbol);
    for (Env* level = local; level; level = level->parent) {
        DEBUG_PRINT_VERBOSE("env_lookup: level: %p\n", level);
        for (Binding *b = level->bindings; b != NULL; b = b->next) {
            if (b->name == name) {
                DEBUG_PRINT_VERBOSE("env_lookup: env: %p, %s = %s, pointer to value: %p\n", level, name->symbol, object_to_string(b->value), b->value);
                return b->value;
            }
        }
    }
    DEBUG_PRINT_VERBOSE("env_lookup: env: %p, %s not found\n", local, name->symbol);
    RAISE_ERROR("env_lookup: %s not found\n", name);
}

Env* push_env(Env *local){
    if(local == NULL){
        RAISE_ERROR("expected enviroment\n");
    }
    Env* new = malloc(sizeof(Env));
    new->marked = false;
    new->bindings = NULL;
    new->parent = local;
    track_envs(new);
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