/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Node evaluation / execution
 */

#include "const.h"
#include "impl.h"
#include "type/Callable.h"
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
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    call = dataOf(call);

    zvalue function = collGet(call, STR_function);
    zvalue actuals = collGet(call, STR_actuals);
    zvalue functionId = execExpression(frame, function);

    zint argCount = collSize(actuals);
    zvalue actualsArr[argCount];
    zvalue args[argCount];

    arrayFromList(actualsArr, actuals);

    // If there are any interpolated arguments, then `interpolateAny` is
    // set to `true`, and `fullCount` indicates the count of arguments
    // after interpolation.
    zint fullCount = 0;
    bool interpolateAny = false;

    for (zint i = 0; i < argCount; i++) {
        zvalue one = actualsArr[i];
        zvalue oneType = typeOf(one);
        bool voidable;
        bool interpolate;
        zvalue eval;

        if (valEq(oneType, STR_voidable)) {
            // We replace the value in `actualsArr` with the voidable
            // payload in order to keep the follow-up interpolation loop
            // simpler.
            one = actualsArr[i] = dataOf(one);
            oneType = typeOf(one);
            voidable = true;
        } else {
            voidable = false;
        }

        if (valEq(oneType, STR_interpolate)) {
            one = dataOf(one);
            interpolate = true;
        } else {
            interpolate = false;
        }

        if (voidable) {
            eval = execExpressionVoidOk(frame, one);
            if (eval == NULL) {
                return NULL;
            }
        } else {
            eval = execExpression(frame, one);
        }

        if (interpolate) {
            eval = constCollectGenerator(eval);
            args[i] = eval;
            fullCount += collSize(eval);
            interpolateAny = true;
        } else {
            args[i] = eval;
            fullCount++;
        }
    }

    if (interpolateAny) {
        zvalue fullArgs[fullCount];
        zint at = 0;

        // Build the flattened argument list.
        for (zint i = 0; i < argCount; i++) {
            zvalue oneNode = actualsArr[i];
            zvalue oneArg = args[i];
            if (hasType(oneNode, STR_interpolate)) {
                arrayFromList(&fullArgs[at], oneArg);
                at += collSize(oneArg);
            } else {
                fullArgs[at] = oneArg;
                at++;
            }
        }

        return funCall(functionId, fullCount, fullArgs);
    } else {
        return funCall(functionId, argCount, args);
    }
}

/**
 * Executes a `varRef` form.
 */
static zvalue execVarRef(Frame *frame, zvalue varRef) {
    zvalue name = dataOf(varRef);
    return frameGet(frame, name);
}

/**
 * Executes an `interpolate` form.
 */
static zvalue execInterpolate(Frame *frame, zvalue interpolate) {
    zvalue result = execExpressionVoidOk(frame, dataOf(interpolate));

    if (result == NULL) {
        die("Attempt to interpolate void.");
    }

    result = constCollectGenerator(result);

    switch (collSize(result)) {
        case 0: return NULL;
        case 1: return collNth(result, 0);
        default: {
            die("Attempt to interpolate multiple values.");
        }
    }
}


/*
 * Module Definitions
 */

/* Documented in header. */
zvalue execExpression(Frame *frame, zvalue expression) {
    zvalue result = execExpressionVoidOk(frame, expression);

    if (result == NULL) {
        die("Invalid use of void expression result.");
    }

    return result;
}

/* Documented in header. */
zvalue execExpressionVoidOk(Frame *frame, zvalue e) {
    switch (evalTypeOf(e)) {
        case EVAL_call:        return execCall(frame, e);
        case EVAL_closure:     return execClosure(frame, e);
        case EVAL_expression:  return execExpressionVoidOk(frame, dataOf(e));
        case EVAL_interpolate: return execInterpolate(frame, e);
        case EVAL_literal:     return dataOf(e);
        case EVAL_varRef:      return execVarRef(frame, e);
        default: {
            die("Invalid expression type: %s", valDebugString(typeOf(e)));
        }
    }
}

/* Documented in header. */
void execVarDef(Frame *frame, zvalue varDef) {
    zvalue nameValue = dataOf(varDef);
    zvalue name = collGet(nameValue, STR_name);
    zvalue valueExpression = collGet(nameValue, STR_value);
    zvalue value = execExpression(frame, valueExpression);

    frameAdd(frame, name, value);
}


/*
 * Exported Definitions
 */

/* Documented in header. */
zvalue langEval0(zvalue context, zvalue node) {
    Frame frame;

    frameInit(&frame, NULL, NULL, context);
    return execExpressionVoidOk(&frame, node);
}
