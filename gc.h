#ifndef GC_H
#define GC_H

#include "object.h"
#include "env.h"
#include "error.h"
#include "debug.h"

void gc_collect(void);

void set_parsing(bool is_parsing);
bool is_parsing(void);


extern Object *allocated_objects; // linked list of allocated objects  
void free_all_objects(void);
void track_object(Object *obj);

#endif