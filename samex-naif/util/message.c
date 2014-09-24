// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

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
void note(const char *format, ...) {
    va_list rest;

    va_start(rest, format);
    vfprintf(stderr, format, rest);
    va_end(rest);
    fputs("\n", stderr);
}

// Documented in header.
void die(const char *format, ...) {
    va_list rest;

    va_start(rest, format);
    vfprintf(stderr, format, rest);
    va_end(rest);
    fputs("\n", stderr);

    // This check prevents infinite recursion in cases where the stack trace
    // output ends up calling `die`.
    if (currentlyDying) {
        fprintf(stderr, "    ...while in the middle of dying. Eek!");
    } else {
        currentlyDying = true;

        // Use a local variable for the stack pointer, since the stringifiers
        // will also manipulate the stack (and may have bugs in same!).
        UtilStackGiblet *stackPtr = utilStackTop;
        while ((stackPtr != NULL) && (stackPtr->magic == UTIL_GIBLET_MAGIC)) {
            if (stackPtr->function != NULL) {
                char *message = stackPtr->function(stackPtr->state);
                fprintf(stderr, "    at %s\n", message);
            }
            stackPtr = stackPtr->pop;
        }
    }

    exit(1);
}
