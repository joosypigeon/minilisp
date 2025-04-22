#ifndef ENV_H
#define ENV_H

#include "object.h"

typedef struct Binding {
    char *name;
    Object *value;
    struct Binding *next;
} Binding;

typedef struct Env {
    Binding *bindings;
    struct Env *parent;
} Env;

extern Env *global;

void env_define(Env *local, const char *name, Object *value);
bool set(Env *local, const char *name, Object *value);
Object *env_lookup(Env *local, const char *name);
Env* push_env(Env *local);
Env* pop_env(Env *local);
void free_all_envs();

#endif // ENV_H