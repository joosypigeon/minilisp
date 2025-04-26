#include "gc.h"

Object *allocated_objects = NULL;
int counter = 0;

int length_of_allocated_objects(void) {
    int count = 0;
    Object *current = allocated_objects;
    while (current) {
        count++;
        current = current->next_allocated;
    }
    return count;
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
    DEBUG_PRINT_VERBOSE("track_object: obj: %s\n", object_to_string(obj));
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

void free_object(Object *obj) {
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
}


void mark_object(Object *obj);

void mark_env(Env *env) {
    while (env) {
        for (Binding *b = env->bindings; b; b = b->next) {
            mark_object(b->value);
        }
        env = env->parent;
    }
}

void mark_object(Object *obj) {
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
    mark_env(global);        // mark from global
    if (current_error)       // mark current error if present
        mark_object(current_error);
    // add more root sets if needed
}

void sweep(void) {
    Object **p = &allocated_objects;
    int freed_count = 0;
    while (*p) {
        Object *obj = *p;
        if (!obj->marked) {
            *p = obj->next_allocated; // unlink
            free_object(obj);
            freed_count++;
        } else {
            obj->marked = false;  // unmark for next cycle
            p = &obj->next_allocated;
        }
    }
    DEBUG_PRINT_INFO("Freed %d unmarked objects\n", freed_count);
}

// Entry point for GC
void gc_collect(void) {
    DEBUG_PRINT_INFO("Garbage collection started\n");
    mark_from_roots();
    sweep();
}
