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
#include "type/List.h"
#include "type/Map.h"
#include "util.h"


/*
 * Private Definitions
 */

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    call = dataOf(call);

    zvalue function = mapGet(call, STR_function);
    zvalue actuals = mapGet(call, STR_actuals);
    zvalue functionId = execExpression(frame, function);

    zint argCount = pbSize(actuals);
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

        if (pbEq(oneType, STR_voidable)) {
            // We replace the value in `actualsArr` with the voidable
            // payload in order to keep the follow-up interpolation loop
            // simpler.
            one = actualsArr[i] = dataOf(one);
            oneType = typeOf(one);
            voidable = true;
        } else {
            voidable = false;
        }

        if (pbEq(oneType, STR_interpolate)) {
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
            fullCount += pbSize(eval);
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
                at += pbSize(oneArg);
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

    switch (pbSize(result)) {
        case 0: return NULL;
        case 1: return listNth(result, 0);
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
    zvalue type = typeOf(e);

    // Switching on the first character of the type is a bit of a hack. It
    // lets us avoid having to have a single big cascading `if` with a lot of
    // `pbEq` calls.
    switch (stringNth(type, 0)) {
        case 'c': {
            if      (pbEq(type, STR_call))    { return execCall(frame, e); }
            else if (pbEq(type, STR_closure)) { return execClosure(frame, e); }
            break;
        }
        case 'e': {
            if (pbEq(type, STR_expression)) {
                return execExpressionVoidOk(frame, dataOf(e));
            }
            break;
        }
        case 'i': {
            if (pbEq(type, STR_interpolate)) {
                return execInterpolate(frame, e);
            }
            break;
        }
        case 'l': {
            if (pbEq(type, STR_literal)) { return dataOf(e); }
            break;
        }
        case 'v': {
            if (pbEq(type, STR_varRef)) { return execVarRef(frame, e); }
            break;
        }
    }

    die("Invalid expression type.");
}

/* Documented in header. */
void execVarDef(Frame *frame, zvalue varDef) {
    zvalue nameValue = dataOf(varDef);
    zvalue name = mapGet(nameValue, STR_name);
    zvalue valueExpression = mapGet(nameValue, STR_value);
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
