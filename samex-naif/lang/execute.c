// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

//
// Node evaluation / execution
//

#include "const.h"
#include "type/Box.h"
#include "type/Class.h"
#include "type/Generator.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "type/Symbol.h"
#include "type/SymbolTable.h"
#include "util.h"

#include "impl.h"
#include "langnode.h"


//
// Private Definitions
//

// Both of these are defined at the bottom of this section.
static zvalue execExpression(Frame *frame, zvalue expression);
static zvalue execExpressionVoidOk(Frame *frame, zvalue e);

/**
 * Executes an `apply` form.
 */
static zvalue execApply(Frame *frame, zvalue apply) {
    zvalue targetExpr = cm_get(apply, SYM(target));
    zvalue nameExpr = cm_get(apply, SYM(name));
    zvalue valuesExpr = cm_get(apply, SYM(values));

    zvalue target = execExpression(frame, targetExpr);
    zvalue name = execExpression(frame, nameExpr);
    zvalue values = execExpressionOrMaybe(frame, valuesExpr);

    return methApply(target, name, values);
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    zvalue targetExpr = cm_get(call, SYM(target));
    zvalue nameExpr = cm_get(call, SYM(name));
    zvalue valuesExprs = cm_get(call, SYM(values));

    zvalue target = execExpression(frame, targetExpr);
    zvalue name = execExpression(frame, nameExpr);

    zint argCount = get_size(valuesExprs);
    zvalue args[argCount];
    arrayFromList(args, valuesExprs);

    // Replace each actual with its evaluation.
    for (zint i = 0; i < argCount; i++) {
        args[i] = execExpression(frame, args[i]);
    }

    return methCall(target, name, argCount, args);
}

/**
 * Executes a `fetch` form.
 */
static zvalue execFetch(Frame *frame, zvalue fetch) {
    zvalue targetExpr = cm_get(fetch, SYM(target));
    zvalue target = execExpression(frame, targetExpr);

    return cm_fetch(target);
}

/**
 * Executes an `import*` form.
 */
static void execImport(Frame *frame, zvalue import) {
    execStatements(frame, makeDynamicImport(import));
}

/**
 * Executes a `maybe` form.
 */
static zvalue execMaybe(Frame *frame, zvalue maybe) {
    zvalue valueExpression = cm_get(maybe, SYM(value));
    return execExpressionVoidOk(frame, valueExpression);
}

/**
 * Executes a `noYield` form.
 */
static void execNoYield(Frame *frame, zvalue noYield)
    __attribute__((noreturn));
static void execNoYield(Frame *frame, zvalue noYield) {
    zvalue valueExpression = cm_get(noYield, SYM(value));
    mustNotYield(execExpression(frame, valueExpression));
}

/**
 * Executes a `store` form.
 */
static zvalue execStore(Frame *frame, zvalue store) {
    zvalue targetExpr = cm_get(store, SYM(target));
    zvalue valueExpr = cm_get(store, SYM(value));
    zvalue target = execExpression(frame, targetExpr);
    zvalue value = execExpressionOrMaybe(frame, valueExpr);

    return cm_store(target, value);
}

/**
 * Executes a `varDef` form, by updating the given execution frame
 * as appropriate.
 */
static void execVarDef(Frame *frame, zvalue varDef) {
    zvalue name = cm_get(varDef, SYM(name));
    zvalue valueExpression = cm_get(varDef, SYM(value));
    zvalue box = valueExpression
        ? cm_new(Result, execExpression(frame, valueExpression))
        : cm_new(Promise);

    frameDef(frame, name, box);
}

/**
 * Executes a `varDefMutable` form, by updating the given execution frame
 * as appropriate.
 */
static void execVarDefMutable(Frame *frame, zvalue varDef) {
    zvalue name = cm_get(varDef, SYM(name));
    zvalue valueExpression = cm_get(varDef, SYM(value));
    zvalue value = valueExpression
        ? execExpression(frame, valueExpression)
        : NULL;

    frameDef(frame, name, cm_new(Cell, value));
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Frame *frame, zvalue varRef) {
    zvalue name = cm_get(varRef, SYM(name));
    return frameGet(frame, name);
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
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
static zvalue execExpressionVoidOk(Frame *frame, zvalue e) {
    switch (recordEvalType(e)) {
        case EVAL_apply:    { return execApply(frame, e);   }
        case EVAL_call:     { return execCall(frame, e);    }
        case EVAL_closure:  { return execClosure(frame, e); }
        case EVAL_fetch:    { return execFetch(frame, e);   }
        case EVAL_literal:  { return cm_get(e, SYM(value)); }
        case EVAL_noYield:  { execNoYield(frame, e);        }
        case EVAL_store:    { return execStore(frame, e);   }
        case EVAL_varRef:   { return execVarRef(frame, e);  }
        default: {
            die("Invalid expression type: %s", cm_debugString(classOf(e)));
        }
    }
}

/**
 * Executes a single `statement` form. Works for `expression` forms too.
 */
static void execStatement(Frame *frame, zvalue statement) {
    switch (recordEvalType(statement)) {
        case EVAL_importModule:          { execImport(frame, statement);           break; }
        case EVAL_importModuleSelection: { execImport(frame, statement);           break; }
        case EVAL_importResource:        { execImport(frame, statement);           break; }
        case EVAL_varDef:                { execVarDef(frame, statement);           break; }
        case EVAL_varDefMutable:         { execVarDefMutable(frame, statement);    break; }
        default:                         { execExpressionVoidOk(frame, statement); break; }
    }
}


//
// Module Definitions
//

// Documented in header.
zvalue execExpressionOrMaybe(Frame *frame, zvalue e) {
    switch (recordEvalType(e)) {
        case EVAL_maybe: { return execMaybe(frame, e);      }
        case EVAL_void:  { return NULL;                     }
        default:         { return execExpression(frame, e); }
    }
}

// Documented in header.
void execStatements(Frame *frame, zvalue statements) {
    zint size = get_size(statements);
    zvalue arr[size];
    arrayFromList(arr, statements);

    for (zint i = 0; i < size; i++) {
        execStatement(frame, arr[i]);
    }
}


//
// Exported Definitions
//

// Documented in header.
zvalue langEval0(zvalue env, zvalue node) {
    zint size = get_size(env);
    zmapping mappings[size];

    arrayFromSymbolTable(mappings, env);
    for (zint i = 0; i < size; i++) {
        mappings[i].value = cm_new(Result, mappings[i].value);
    }
    env = symbolTableFromArray(size, mappings);

    Frame frame;
    frameInit(&frame, NULL, NULL, env);

    return execExpressionOrMaybe(&frame, node);
}
