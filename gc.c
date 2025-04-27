#include "gc.h"

Object *allocated_objects = NULL;
int counter = 0;

Env *allocated_envs = NULL;
int env_counter = 0;

void reset_envs(void) {
    Env *current = allocated_envs;
    while (current) {
        current->marked = false; // Reset marked for each env
        current = current->next_allocated;
    }
}

int length_of_allocated_objects(void) {
    int count = 0;
    Object *current = allocated_objects;
    while (current) {
        count++;
        current = current->next_allocated;
    }
    return count;
}

Object *check_for_symbol(const char *symbol) {
    Object *current = allocated_objects;
    while (current) {
        if (current->type == TYPE_SYMBOL && strcmp(current->symbol, symbol) == 0) {
            return current;
        }
        current = current->next_allocated;
    }
    return NULL;
}


bool parsing = false;   
void set_parsing(bool is_parsing) {
    parsing = is_parsing;
}
bool is_parsing(void) {
    return parsing;
}

void gc_collect(void);

void track_object(Object *obj) {
    DEBUG_PRINT_VERBOSE("track_object: obj: %s, the pointer: %p\n", object_to_string(obj), obj);
    obj->next_allocated = allocated_objects;
    allocated_objects = obj;
    obj->marked = false; // Initialize marked to false
    counter++;
    /*
    if ( counter == 1000000) {
        gc_collect();
        counter = 0;
    }*/
}

void track_envs(Env *env) {
    DEBUG_PRINT_VERBOSE("track_envs: env: %p\n", env);
    env->next_allocated = allocated_envs;
    allocated_envs = env;
    env->marked = false; // Initialize marked to false
    env_counter++;
}

void free_object(Object *obj) {
    DEBUG_PRINT_VERBOSE("free_object: obj: %s, the pointer: %p\n", object_to_string(obj), obj);
    if (!obj) return;

    switch (obj->type) {
        case TYPE_INT:
            mpz_clear(obj->int_val);
            break;
        case TYPE_SYMBOL:
        case TYPE_STRING:
            free(obj->symbol); // or .str_val
            break;
        case TYPE_ERROR:
            free(obj->error_msg);
            break;
        case TYPE_PAIR:
            // Do not recursively free car/cdr — just the container.
            break;
        case TYPE_LAMBDA:
            // Don't free .params/body/env — owned elsewhere.
            break;
        default:
            break;
    }

    free(obj);
}

void free_all_objects() {
    DEBUG_PRINT_INFO("free_all_objects:enter:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
    int count = 0;
    Object *current = allocated_objects;
    while (current) {
        Object *next = current->next_allocated;
        free_object(current);
        current = next;
        count++;
    }
    allocated_objects = NULL;
    DEBUG_PRINT_INFO("Freed %d objects\n", count);
    DEBUG_PRINT_INFO("free_all_objects:leave:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
}


void mark_object(Object *obj);

void mark_env(Env *env) {
    DEBUG_PRINT_VERBOSE("XXXXXXXXXXXXXXXXmark_env: env: %p XXXXXXXXXXXXXXXX\n", env);
    while (env) {
        if (env->marked) return;
        env->marked = true;

        for (Binding *b = env->bindings; b; b = b->next) {
            mark_object(b->name);
            mark_object(b->value);
            DEBUG_PRINT_VERBOSE("mark_env: binding: %s with marking %s, value: %s with marking: %s\n", b->name->symbol, b->name->marked ? "true" : "false", object_to_string(b->value), b->value->marked ? "true" : "false");
        }
        env = env->parent;
    }
}

void display_bindings(Env *env) {
    DEBUG_PRINT_VERBOSE("display_bindings:enter:-------------------------------------\n");
    while (env) {
        DEBUG_PRINT_VERBOSE("display_bindings: env: %p\n", env);
        for (Binding *b = env->bindings; b; b = b->next) {
            DEBUG_PRINT_VERBOSE("display_bindings: binding: %s with marked %s, value: %s with marked: %s\n", b->name->symbol, b->name->marked ? "true" : "false", object_to_string(b->value), b->value->marked ? "true" : "false");
        }
        env = env->parent;
    }
    DEBUG_PRINT_VERBOSE("display_bindings:leave:--------------------------------------\n");
}

void display_allocated_objects(void) {
    DEBUG_PRINT_VERBOSE("display_allocated_objects:enter:*******************************\n");
    Object *current = allocated_objects;
    while (current) {
        DEBUG_PRINT_VERBOSE("display_allocated_objects: object: %s, the pointer: %p, marked: %s\n", object_to_string(current), current, current->marked ? "true" : "false");
        current = current->next_allocated;
    }
    DEBUG_PRINT_VERBOSE("display_allocated_objects:leave:*******************************\n");
}

void mark_object(Object *obj) {
    DEBUG_PRINT_VERBOSE("mark_object: obj: %s, the pointer: %p\n", object_to_string(obj), obj);
    if (!obj || obj->marked) return;
    obj->marked = true;

    switch (obj->type) {
        case TYPE_PAIR:
            mark_object(obj->car);
            mark_object(obj->cdr);
            break;
        case TYPE_LAMBDA:
            mark_object(obj->lambda.params);
            mark_object(obj->lambda.body);
            mark_env(obj->lambda.env);
            break;
        default:
            break; // Other types don't have pointers
    }
}

void mark_from_roots(void) {
    DEBUG_PRINT_VERBOSE("mark_from_roots:enter:++++++++++++++++++++++\n");
    // Mark all objects from the root set
    display_allocated_objects();
    display_bindings(global);
    mark_env(global);        // mark from global
    display_bindings(global);
    display_allocated_objects();
    if (current_error)       // mark current error if present
        mark_object(current_error);
    display_bindings(global);
    display_allocated_objects();
    // add more root sets if needed
    DEBUG_PRINT_VERBOSE("mark_from_roots:leave:++++++++++++++++++++++\n");
}

void sweep(void) {
    Object **p = &allocated_objects;
    Object *previous = NULL;
    int freed_count = 0;
    while (*p) {
        Object *obj = *p;
        if (!obj->marked) {
            *p = obj->next_allocated; // unlink
            DEBUG_PRINT_VERBOSE("sweep: freeing object: %s, the pointer: %p\n", object_to_string(obj), obj);
            free_object(obj);
            freed_count++;
            if (previous)
                previous->next_allocated = *p; // link previous to next
        } else {
            DEBUG_PRINT_VERBOSE("sweep: keeping object: %s, the pointer: %p\n", object_to_string(obj), obj);
            obj->marked = false;  // unmark for next cycle
            p = &obj->next_allocated;
        }
    }
    DEBUG_PRINT_INFO("Freed %d unmarked objects\n", freed_count);
}

void free_env(Env *env);

void free_all_envs() {
    DEBUG_PRINT_VERBOSE("free_all_envs:enter\n");
    int count = 0;
    Env *cur = allocated_envs;
    while (cur) {
        Env *next = cur->next_allocated;
        free(cur);
        cur = next;
        count++;
    }
    allocated_envs = NULL;
    DEBUG_PRINT_INFO("Freed %d envs\n", count);
    DEBUG_PRINT_VERBOSE("free_all_env:leave\n");
}

void sweep_envs(void) {
    Env **p = &allocated_envs;
    int freed_count = 0;
    while (*p) {
        Env *env = *p;
        if (!env->marked) {
            *p = env->next_allocated;      // unlink
            free(env);        // free Env
            freed_count++;
        } else {
            env->marked = false;  // reset for next GC
            p = &env->next_allocated;
        }
    }
    DEBUG_PRINT_INFO("Freed %d unmarked environments\n", freed_count);
}

// Entry point for GC
void gc_collect(void) {
    DEBUG_PRINT_INFO("Garbage collection started\n");
    mark_from_roots();
    sweep();
    sweep_envs();
}
