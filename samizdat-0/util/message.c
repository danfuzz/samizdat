/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "util.h"
#include "zlimits.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


/*
 * Helper definitions
 */

/** Death context stack element. */
typedef struct Context {
    /** Function to call. */
    zcontextFunction function;

    /** State argument to pass `function`. */
    void *state;
} Context;

/** Context stack. */
static Context contextStack[UTIL_MAX_CALL_STACK_DEPTH];

/** Current stack depth. */
static zint stackDepth = 0;


/*
 * Exported functions
 */

/* Documented in header. */
void note(const char *format, ...) {
    va_list rest;

    va_start(rest, format);
    vfprintf(stderr, format, rest);
    va_end(rest);
    fputs("\n", stderr);
}

/* Documented in header. */
void die(const char *format, ...) {
    va_list rest;

    va_start(rest, format);
    vfprintf(stderr, format, rest);
    va_end(rest);
    fputs("\n", stderr);

    while (stackDepth > 0) {
        stackDepth--;
        Context *ctx = &contextStack[stackDepth];
        char *message = ctx->function(ctx->state);
        fprintf(stderr, "    at %s\n", message);
    }

    exit(1);
}

/* Documented in header. */
void debugPush(zcontextFunction function, void *state) {
    if (stackDepth == UTIL_MAX_CALL_STACK_DEPTH) {
        die("Maximum stack depth exceeded.");
    }

    contextStack[stackDepth].function = function;
    contextStack[stackDepth].state = state;
    stackDepth++;
}

/* Documented in header. */
void debugPop(void) {
    if (stackDepth <= 0) {
        die("Stack underflow.");
    }

    stackDepth--;
}

/* Documented in header. */
zint debugMark(void) {
    return stackDepth;
}

/* Documented in header. */
void debugReset(zint mark) {
    if (mark > stackDepth) {
        die("Stack can't get deeper on reset.");
    }

    stackDepth = mark;
}
