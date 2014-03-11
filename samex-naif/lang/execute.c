/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Node evaluation / execution
 */

#include "const.h"
#include "impl.h"
#include "type/Box.h"
#include "type/Function.h"
#include "type/Generator.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"
#include "util.h"


/*
 * Private Definitions
 */

/**
 * Gets the `value` binding out of the given node's data payload.
 */
static zvalue valueOf(zvalue node) {
    return collGet(dataOf(node), STR_value);
}

/**
 * Executes an `expression` form, with the result never allowed to be
 * `void`.
 */
static zvalue execExpression(Frame *frame, zvalue expression) {
    zvalue result = execExpressionVoidOk(frame, expression);

    if (result == NULL) {
        die("Invalid use of void expression result.");
    }

    return result;
}

/**
 * Executes an `apply` form.
 */
static zvalue execApply(Frame *frame, zvalue apply) {
    apply = dataOf(apply);

    zvalue functionExpr = collGet(apply, STR_function);
    zvalue actualsExpr = collGet(apply, STR_actuals);
    zvalue function = execExpression(frame, functionExpr);
    zvalue actuals = execExpression(frame, actualsExpr);

    return funApply(function, actuals);
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    call = dataOf(call);

    zvalue functionExpr = collGet(call, STR_function);
    zvalue actualsExprs = collGet(call, STR_actuals);
    zvalue function = execExpression(frame, functionExpr);

    zint argCount = collSize(actualsExprs);
    zvalue args[argCount];
    arrayFromList(args, actualsExprs);

    // Replace each actual with its evaluation.
    for (zint i = 0; i < argCount; i++) {
        args[i] = execExpression(frame, args[i]);
    }

    return funCall(function, argCount, args);
}

/**
 * Executes a `jump` form.
 */
static void execJump(Frame *frame, zvalue jump)
    __attribute__((noreturn));
static void execJump(Frame *frame, zvalue jump) {
    jump = dataOf(jump);

    zvalue functionExpr = collGet(jump, STR_function);
    zvalue argExpr = collGet(jump, STR_value);
    zvalue function = execExpression(frame, functionExpr);
    zvalue arg = (argExpr == NULL)
        ? NULL
        : execExpressionVoidOk(frame, argExpr);

    funJump(function, arg);
}

/* Documented in header. */
static zvalue execVarBind(Frame *frame, zvalue varBind) {
    zvalue nameValue = dataOf(varBind);
    zvalue name = collGet(nameValue, STR_name);
    zvalue valueExpression = collGet(nameValue, STR_value);
    zvalue value = execExpression(frame, valueExpression);

    frameBind(frame, name, value);
    return value;
}

/**
 * Executes a `varDef` form, by updating the given execution frame
 * as appropriate.
 */
static void execVarDef(Frame *frame, zvalue varDef) {
    zvalue nameValue = dataOf(varDef);
    zvalue name = collGet(nameValue, STR_name);
    zvalue valueExpression = collGet(nameValue, STR_value);
    zvalue value = valueExpression
        ? execExpression(frame, valueExpression)
        : NULL;

    frameDef(frame, false, name, value);
}

/**
 * Executes a `varDefMutable` form, by updating the given execution frame
 * as appropriate.
 */
static void execVarDefMutable(Frame *frame, zvalue varDef) {
    zvalue nameValue = dataOf(varDef);
    zvalue name = collGet(nameValue, STR_name);
    zvalue valueExpression = collGet(nameValue, STR_value);
    zvalue value = valueExpression
        ? execExpression(frame, valueExpression)
        : NULL;

    frameDef(frame, true, name, value);
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Frame *frame, zvalue varRef) {
    zvalue name = collGet(dataOf(varRef), STR_name);
    return frameGet(frame, name);
}


/*
 * Module Definitions
 */

/* Documented in header. */
zvalue execExpressionVoidOk(Frame *frame, zvalue e) {
    switch (evalTypeOf(e)) {
        case EVAL_apply:   return execApply(frame, e);
        case EVAL_call:    return execCall(frame, e);
        case EVAL_closure: return execClosure(frame, e);
        case EVAL_jump:    execJump(frame, e);
        case EVAL_literal: return valueOf(e);
        case EVAL_varBind: return execVarBind(frame, e);
        case EVAL_varRef:  return execVarRef(frame, e);
        default: {
            die("Invalid expression type: %s", valDebugString(typeOf_new(e)));
        }
    }
}

/* Documented in header. */
void execStatement(Frame *frame, zvalue statement) {
    switch (evalTypeOf(statement)) {
        case EVAL_varDef:        execVarDef(frame, statement);           break;
        case EVAL_varDefMutable: execVarDefMutable(frame, statement);    break;
        default:                 execExpressionVoidOk(frame, statement); break;
    }
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue langEval0(zvalue context, zvalue node) {
    Frame frame;

    zint size = collSize(context);
    zmapping mappings[size];

    arrayFromMap(mappings, context);
    for (zint i = 0; i < size; i++) {
        mappings[i].value = makeResult(mappings[i].value);
    }
    context = mapFromArray(size, mappings);

    frameInit(&frame, NULL, NULL, context);
    return execExpressionVoidOk(&frame, node);
}
