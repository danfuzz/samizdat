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
    /** Variables bound at this level. */
    zvalue locals;

    /** Pending return value. */
    zvalue toReturn;

    /** Parent context. */
    struct Context *parent;

    /** Magic function. */
    zmagic magic;
}
Context;


/*
 * Helper functions
 */

/* Defined below. */
static zvalue execExpression(Context *context, zvalue expression);

/**
 * Executes a `call` form.
 */
static void execCall(Context *context, zvalue call) {
    assertType(call, STR_CALL);

    samDie("TODO");
}

/**
 * Executes a `function` form.
 */
static void execFunction(Context *context, zvalue function) {
    assertType(function, STR_FUNCTION);

    samDie("TODO");
}

/**
 * Executes a `uniqlet` form.
 */
static void execUniqlet(Context *context, zvalue uniqlet) {
    assertType(uniqlet, STR_UNIQLET);

    samDie("TODO");
}

/**
 * Executes a `maplet` form.
 */
static void execMaplet(Context *context, zvalue maplet) {
    assertType(maplet, STR_MAPLET);

    samDie("TODO");
}

/**
 * Executes a `listlet` form.
 */
static void execListlet(Context *context, zvalue listlet) {
    assertType(listlet, STR_LISTLET);

    samDie("TODO");
}

/**
 * Executes a `varRef` form.
 */
static void execVarRef(Context *context, zvalue varRef) {
    assertType(varRef, STR_VAR_REF);

    samDie("TODO");
}

/**
 * Executes a `literal` form.
 */
static void execLiteral(Context *context, zvalue literal) {
    assertType(literal, STR_LITERAL);

    samDie("TODO");
}

/**
 * Executes an `expression` form.
 */
static zvalue execExpression(Context *context, zvalue expression) {
    assertType(expression, STR_EXPRESSION);

    samDie("TODO");
}

/**
 * Executes a `varDef` form.
 */
static void execVarDef(Context *context, zvalue varDef) {
    assertType(varDef, STR_VAR_DEF);

    zvalue nameValue = highValue(varDef);
    zvalue name = samMapletGet(nameValue, STR_NAME);
    zvalue value = samMapletGet(nameValue, STR_VALUE);

    if (samMapletGet(context->locals, name) != NULL) {
        samDie("Duplicate assignment.");
    }

    context->locals =
        samMapletPut(context->locals, name, execExpression(context, value));
}

/**
 * Executes a `return` form.
 */
static void execReturn(Context *context, zvalue returnForm) { 
    assertType(returnForm, STR_RETURN);

    zvalue result = execExpression(context, highValue(returnForm));

    if (context->toReturn == NULL) {
        context->toReturn = result;
    }
}

/**
 * Executes a `statements` form.
 */
static void execStatements(Context *context, zvalue statements) {
    assertType(statements, STR_STATEMENTS);

    statements = highValue(statements);
    zint size = samSize(statements);

    for (zint i = 0; (i < size) && (context->toReturn == NULL); i++) {
        zvalue one = samListletGet(statements, i);
        zvalue type = highType(one);
        if (hasType(one, STR_EXPRESSION)) {
            execExpression(context, one);
        } else if (hasType(one, STR_VAR_DEF)) {
            execVarDef(context, one);
        } else if (hasType(one, STR_RETURN)) {
            execReturn(context, one);
        } else {
            samDie("Invalid statements element.");
        }
    }
}

/*
 * Exported functions
 */

/** Documented in API header. */
zvalue samExecute(zvalue environment, zmagic magic, zvalue code) {
    samAssertMaplet(environment);

    Context context = { environment, NULL, NULL, magic };
    execStatements(&context, code);

    return context.locals;
}
