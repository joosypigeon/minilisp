
#ifndef ERROR_H
#define ERROR_H

#include <setjmp.h>
#include "object.h"

extern jmp_buf eval_env;
extern Object *current_error;

#define RAISE_ERROR(fmt, ...) do {                                                \
    current_error = make_error("%s:%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
    longjmp(eval_env, 1);                                                         \
} while (0)

#endif // ERROR_H