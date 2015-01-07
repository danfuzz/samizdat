// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "impl.h"


//
// Private Definitions
//

/** Death context stack element. */
typedef struct Context {
    /** Function to call. */
    zcontextFunction function;

    /** State argument to pass `function`. */
    void *state;
} Context;

// Documented in header.
UtilStackGiblet *utilStackTop = NULL;

/** Whether death is currently in progress. */
static bool currentlyDying = false;


//
// Exported Definitions
//

// Documented in header.
void die(const char *format, ...) {
    va_list rest;

    va_start(rest, format);
    char *str = utilVFormat(format, rest);
    fputs(str, stderr);
    fputs("\n", stderr);
    utilFree(str);
    va_end(rest);

    // This check prevents infinite recursion in cases where the stack trace
    // output ends up calling `die`.
    if (currentlyDying) {
        fputs("    ...while in the middle of dying. Eek!\n", stderr);
    } else {
        currentlyDying = true;

        // Use a local variable for the stack pointer, since the stringifiers
        // will also manipulate the stack (and may have bugs!).
        UtilStackGiblet *stackPtr = utilStackTop;
        while ((stackPtr != NULL) && (stackPtr->magic == UTIL_GIBLET_MAGIC)) {
            if (stackPtr->function != NULL) {
                char *message = stackPtr->function(stackPtr->state);
                fputs("    at ", stderr);
                fputs(message, stderr);
                fputs("\n", stderr);
            }
            stackPtr = stackPtr->pop;
        }
    }

    exit(1);
}

// Documented in header.
void note(const char *format, ...) {
    va_list rest;

    va_start(rest, format);

    if (strcmp(format, "%s") == 0) {
        // Avoid the parsing overhead for a simple literal string. This is how
        // this function gets called from the in-language `note()` function.
        const char *str = va_arg(rest, const char *);
        fputs(str, stderr);
    } else {
        char *str = utilVFormat(format, rest);
        fputs(str, stderr);
        utilFree(str);
    }

    va_end(rest);

    fputs("\n", stderr);
}
