/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "util.h"

// TODO: Maybe also use `__attribute((weak))`.
static void run(void) __attribute__((constructor));
static void run(void) {
    note("=== yo");
}
