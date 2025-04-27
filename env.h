#ifndef ENV_H
#define ENV_H

#include "object.h"

typedef struct Binding {
    Object *name;
    Object *value;
    struct Binding *next;
} Binding;

typedef struct Env {
    bool marked;
    Env *next_allocated;
    Binding *bindings;
    struct Env *parent;
} Env;

extern Env *global;

void env_define(Env *local, Object *name, Object *value);
bool set(Env *local, Object *name, Object *value);
Object *env_lookup(Env *local, Object *name);
Env* push_env(Env *local);
Env* pop_env(Env *local);
void free_all_envs();

#endif // ENV_H