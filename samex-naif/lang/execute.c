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
    zvalue targetExpr, nameExpr, valuesExpr;
    if (!recGet3(apply,
            SYM(target), &targetExpr,
            SYM(name),   &nameExpr,
            SYM(values), &valuesExpr)) {
        die("Invalid `apply` node.");
    }

    zvalue target = execExpression(frame, targetExpr);
    zvalue name = execExpression(frame, nameExpr);
    zvalue values = execExpressionOrMaybe(frame, valuesExpr);

    return methApply(target, name, values);
}

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    zvalue targetExpr, nameExpr, valuesList;
    if (!recGet3(call,
            SYM(target), &targetExpr,
            SYM(name),   &nameExpr,
            SYM(values), &valuesList)) {
        die("Invalid `call` node.");
    }

    zarray valuesArr = zarrayFromList(valuesList);
    zvalue target = execExpression(frame, targetExpr);
    zvalue name = execExpression(frame, nameExpr);

    // Evaluate each argument expression.
    zint argCount = valuesArr.size;
    zvalue args[argCount];
    for (zint i = 0; i < argCount; i++) {
        args[i] = execExpression(frame, valuesArr.elems[i]);
    }

    return methCall(target, name, (zarray) {argCount, args});
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
    zvalue targetExpr, valueExpr;
    if (!recGet2(store,
            SYM(target), &targetExpr,
            SYM(value),  &valueExpr)) {
        die("Invalid `store` node.");
    }

    zvalue target = execExpression(frame, targetExpr);
    zvalue value = execExpressionOrMaybe(frame, valueExpr);

    return cm_store(target, value);
}

/**
 * Executes a `varDef` form, by updating the given execution frame
 * as appropriate.
 */
static void execVarDef(Frame *frame, zvalue varDef) {
    zvalue box, name, valueExpr;
    if (!recGet3(varDef,
            SYM(box),   &box,
            SYM(name),  &name,
            SYM(value), &valueExpr)) {
        die("Invalid `varDef` node.");
    }

    zvalue cls;
    switch(symbolEvalType(box)) {
        case EVAL_cell:    { cls = CLS_Cell;    break; }
        case EVAL_lazy:    { cls = CLS_Lazy;    break; }
        case EVAL_promise: { cls = CLS_Promise; break; }
        case EVAL_result:  { cls = CLS_Result;  break; }
        default: {
            die("Bogus `box` value: %s", cm_debugString(box));
        }
    }

    zvalue value = execExpressionOrMaybe(frame, valueExpr);
    zvalue boxInstance = (value == NULL)
        ? METH_CALL(cls, new)
        : METH_CALL(cls, new, value);

    frameDef(frame, name, boxInstance);
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Frame *frame, zvalue varRef) {
    zvalue name = cm_get(varRef, SYM(name));
    return frameGet(frame, name);
}

/**
 * Helper for `execExpression*` which does the dispatch given a known
 * `zevalType`.
 */
static zvalue execExpr0(Frame *frame, zvalue e, zevalType type, bool voidOk) {
    zvalue result;
    switch (type) {
        case EVAL_apply:   { result = execApply(frame, e);   break; }
        case EVAL_call:    { result = execCall(frame, e);    break; }
        case EVAL_closure: { result = execClosure(frame, e); break; }
        case EVAL_fetch:   { result = execFetch(frame, e);   break; }
        case EVAL_literal: { result = cm_get(e, SYM(value)); break; }
        case EVAL_noYield: { execNoYield(frame, e);                 }
        case EVAL_store:   { result = execStore(frame, e);   break; }
        case EVAL_varRef:  { result = execVarRef(frame, e);  break; }
        default: {
            die("Invalid expression type: %s", cm_debugString(classOf(e)));
        }
    }

    if (!voidOk && (result == NULL)) {
        die("Invalid use of void expression result.");
    }

    return result;
}

/**
 * Executes an `expression` form, with the result never allowed to be
 * `void`.
 */
static zvalue execExpression(Frame *frame, zvalue expression) {
    return execExpr0(frame, expression, recordEvalType(expression), false);
}

/**
 * Executes an `expression` form, with the result possibly being
 * `void` (represented as `NULL`).
 */
static zvalue execExpressionVoidOk(Frame *frame, zvalue expression) {
    return execExpr0(frame, expression, recordEvalType(expression), true);
}

/**
 * Executes a single `statement` form. Works for `expression` forms too.
 */
static void execStatement(Frame *frame, zvalue s) {
    zevalType type = recordEvalType(s);
    switch (type) {
        case EVAL_importModule:
        case EVAL_importModuleSelection:
        case EVAL_importResource: { execImport(frame, s);            break; }
        case EVAL_varDef:         { execVarDef(frame, s);            break; }
        default:                  { execExpr0(frame, s, type, true); break; }
    }
}


//
// Module Definitions
//

// Documented in header.
zvalue execExpressionOrMaybe(Frame *frame, zvalue e) {
    zevalType type = recordEvalType(e);
    switch (type) {
        case EVAL_maybe: {
            return execExpressionVoidOk(frame, cm_get(e, SYM(value)));
        }
        case EVAL_void:  {
            return NULL;
        }
        default: {
            return execExpr0(frame, e, type, false);
        }
    }
}

// Documented in header.
void execStatements(Frame *frame, zvalue statements) {
    zarray arr = zarrayFromList(statements);

    for (zint i = 0; i < arr.size; i++) {
        execStatement(frame, arr.elems[i]);
    }
}


//
// Exported Definitions
//

// Documented in header.
zvalue langEval0(zvalue env, zvalue node) {
    zint size = get_size(env);
    zmapping mappings[size];

    arrayFromSymtab(mappings, env);
    for (zint i = 0; i < size; i++) {
        mappings[i].value = cm_new(Result, mappings[i].value);
    }
    env = symtabFromArray(size, mappings);

    Frame frame;
    frameInit(&frame, NULL, NULL, env);

    return execExpressionOrMaybe(&frame, node);
}
