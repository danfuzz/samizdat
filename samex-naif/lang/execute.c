/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Node evaluation / execution
 */

#include "const.h"
#include "helpers.h"
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
    zvalue functionExpr = get(apply, STR_function);
    zvalue actualsExpr = get(apply, STR_actuals);
    zvalue function = execExpression(frame, functionExpr);

    if (actualsExpr == NULL) {
        // If `actuals` isn't present, then it turns into a simple
        // no-argument function call.
        return funCall(function, 0, NULL);
    } else {
        zvalue actuals = execExpression(frame, actualsExpr);
        return funApply(function, actuals);
    }
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    zvalue functionExpr = get(call, STR_function);
    zvalue actualsExprs = get(call, STR_actuals);
    zvalue function = execExpression(frame, functionExpr);

    zint argCount = get_size(actualsExprs);
    zvalue args[argCount];
    arrayFromList(args, actualsExprs);

    // Replace each actual with its evaluation.
    for (zint i = 0; i < argCount; i++) {
        args[i] = execExpression(frame, args[i]);
    }

    return funCall(function, argCount, args);
}

/**
 * Executes an `import*` form.
 */
static void execImport(Frame *frame, zvalue import) {
    execStatements(frame, makeDynamicImport(import));
}

/**
 * Executes a `jump` form.
 */
static void execJump(Frame *frame, zvalue jump)
    __attribute__((noreturn));
static void execJump(Frame *frame, zvalue jump) {
    zvalue functionExpr = get(jump, STR_function);
    zvalue argExpr = get(jump, STR_value);
    zvalue function = execExpression(frame, functionExpr);
    zvalue arg = (argExpr == NULL)
        ? NULL
        : execExpressionVoidOk(frame, argExpr);

    funJump(function, arg);
}

/* Documented in header. */
static zvalue execVarBind(Frame *frame, zvalue varBind) {
    zvalue name = get(varBind, STR_name);
    zvalue valueExpression = get(varBind, STR_value);
    zvalue value = execExpression(frame, valueExpression);

    frameBind(frame, name, value);
    return value;
}

/**
 * Executes a `varDef` form, by updating the given execution frame
 * as appropriate.
 */
static void execVarDef(Frame *frame, zvalue varDef) {
    zvalue name = get(varDef, STR_name);
    zvalue valueExpression = get(varDef, STR_value);
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
    zvalue name = get(varDef, STR_name);
    zvalue valueExpression = get(varDef, STR_value);
    zvalue value = valueExpression
        ? execExpression(frame, valueExpression)
        : NULL;

    frameDef(frame, true, name, value);
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Frame *frame, zvalue varRef) {
    zvalue name = get(varRef, STR_name);
    return frameGet(frame, name);
}


/*
 * Module Definitions
 */

/* Documented in header. */
zvalue execExpressionVoidOk(Frame *frame, zvalue e) {
    switch (get_evalType(e)) {
        case EVAL_apply:   return execApply(frame, e);
        case EVAL_call:    return execCall(frame, e);
        case EVAL_closure: return execClosure(frame, e);
        case EVAL_jump:    execJump(frame, e);
        case EVAL_literal: return get(e, STR_value);
        case EVAL_varBind: return execVarBind(frame, e);
        case EVAL_varRef:  return execVarRef(frame, e);
        default: {
            die("Invalid expression type: %s", valDebugString(get_type(e)));
        }
    }
}

/* Documented in header. */
void execStatement(Frame *frame, zvalue statement) {
    switch (get_evalType(statement)) {
        case EVAL_importModule:          execImport(frame, statement);           break;
        case EVAL_importModuleSelection: execImport(frame, statement);           break;
        case EVAL_importResource:        execImport(frame, statement);           break;
        case EVAL_varDef:                execVarDef(frame, statement);           break;
        case EVAL_varDefMutable:         execVarDefMutable(frame, statement);    break;
        default:                         execExpressionVoidOk(frame, statement); break;
    }
}

/* Documented in header. */
void execStatements(Frame *frame, zvalue statements) {
    zint size = get_size(statements);
    zvalue arr[size];
    arrayFromList(arr, statements);

    for (zint i = 0; i < size; i++) {
        execStatement(frame, arr[i]);
    }
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue langEval0(zvalue env, zvalue node) {
    Frame frame;

    zint size = get_size(env);
    zmapping mappings[size];

    arrayFromMap(mappings, env);
    for (zint i = 0; i < size; i++) {
        mappings[i].value = makeResult(mappings[i].value);
    }
    env = mapFromArray(size, mappings);

    frameInit(&frame, NULL, NULL, env);
    return execExpressionVoidOk(&frame, node);
}
