/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


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

    exit(1);
}
