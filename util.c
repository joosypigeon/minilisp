#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "util.h"

char *xstrdup(const char *s) {
    if (s == NULL) {
        fprintf(stderr, "strdup: NULL pointer received\n");
        return NULL;
    }

    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (!copy) {
        fprintf(stderr, "strdup: malloc failed\n");
        return NULL;
    }

    memcpy(copy, s, len);
    return copy;
}