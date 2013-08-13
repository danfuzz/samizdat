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
#include "util.h"

#include <stddef.h>


/*
 * Private Definitions
 */

/**
 * Executes a `call` form.
 */
static zvalue execCall(Frame *frame, zvalue call) {
    call = dataOf(call);

    zvalue function = mapGet(call, STR_FUNCTION);
    zvalue actuals = mapGet(call, STR_ACTUALS);
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

        if (pbEq(oneType, STR_VOIDABLE)) {
            // We replace the value in `actualsArr` with the voidable
            // payload in order to keep the follow-up interpolation loop
            // simpler.
            one = actualsArr[i] = dataOf(one);
            oneType = typeOf(one);
            voidable = true;
        } else {
            voidable = false;
        }

        if (pbEq(oneType, STR_INTERPOLATE)) {
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
            if (hasType(oneNode, STR_INTERPOLATE)) {
                arrayFromList(&fullArgs[at], oneArg);
                at += pbSize(oneArg);
            } else {
                fullArgs[at] = oneArg;
                at++;
            }
        }

        return fnCall(functionId, fullCount, fullArgs);
    } else {
        return fnCall(functionId, argCount, args);
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

    // Switching on the size of the type is a bit of a hack. It lets us
    // avoid having to have a single big cascading `if` with a lot of
    // `pbEq` calls.
    switch (pbSize(type)) {
        case 4: {
            if (pbEq(type, STR_CALL))
                return execCall(frame, e);
            break;
        }
        case 6: {
            if (pbEq(type, STR_VAR_REF))
                return execVarRef(frame, e);
            break;
        }
        case 7: {
            if (pbEq(type, STR_LITERAL))
                return dataOf(e);
            else if (pbEq(type, STR_CLOSURE))
                return execClosure(frame, e);
            break;
        }
        case 10: {
            if (pbEq(type, STR_EXPRESSION))
                return execExpressionVoidOk(frame, dataOf(e));
            break;
        }
        case 11: {
            if (pbEq(type, STR_INTERPOLATE))
                return execInterpolate(frame, e);
            break;
        }
    }

    die("Invalid expression type.");
}

/* Documented in header. */
void execVarDef(Frame *frame, zvalue varDef) {
    zvalue nameValue = dataOf(varDef);
    zvalue name = mapGet(nameValue, STR_NAME);
    zvalue valueExpression = mapGet(nameValue, STR_VALUE);
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
