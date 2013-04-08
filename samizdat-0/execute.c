/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "consts.h"
#include "execute.h"

#include <stddef.h>


/**
 * Execution context.
 */
typedef struct Context {
    /** Parent context. */
    struct Context *parent;

    /** Variables bound at this level. */
    zvalue locals;

    /** Magic function. */
    zmagic magic;
}
Context;


/*
 * Helper functions
 */

/**
 * Executes a `statements` form.
 */
static void execStatements(Context *context, zvalue statements) {
    assertType(statements, STR_STATEMENTS);

    samDie("TODO");
}

/*
 * Exported functions
 */

/** Documented in API header. */
zvalue samExecute(zvalue environment, zmagic magic, zvalue code) {
    samAssertMaplet(environment);

    Context context = { NULL, environment, magic };
    execStatements(&context, code);

    return context.locals;
}
