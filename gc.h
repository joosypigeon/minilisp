#ifndef GC_H
#define GC_H

#include "object.h"
#include "env.h"
#include "error.h"
#include "debug.h"

void gc_collect(void);

void set_parsing(bool is_parsing);
bool is_parsing(void);

void free_all_objects(void);
void free_all_envs(void);
void track_object(Object *obj);
void track_envs(Env *env);

Object *check_for_symbol(const char *symbol);

#endif