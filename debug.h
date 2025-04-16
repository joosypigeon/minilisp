#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

// Define debug levels using an enum for clarity
#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_WARN    2
#define DEBUG_LEVEL_INFO    3
#define DEBUG_LEVEL_VERBOSE 4

// Default level if none is set before inclusion
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_NONE
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_ERROR
    #define DEBUG_PRINT_ERROR(fmt, ...) \
        printf("ERROR   [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT_ERROR(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_WARN
    #define DEBUG_PRINT_WARN(fmt, ...) \
        printf("WARNING [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT_WARN(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
    #define DEBUG_PRINT_INFO(fmt, ...) \
        printf("INFO    [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT_INFO(fmt, ...) ((void)0)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_VERBOSE
    #define DEBUG_PRINT_VERBOSE(fmt, ...) \
        printf("VERBOSE [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT_VERBOSE(fmt, ...) ((void)0)
#endif

#define _STR_HELPER(x) #x
#define _STR(x) _STR_HELPER(x)

#endif // DEBUG_H
