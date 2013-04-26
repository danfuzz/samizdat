/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "const.h"
#include "impl.h"
#include "util.h"

#include <stddef.h>
#include <string.h>


/*
 * Helper definitions
 */

/**
 * Object, that is, a function and its associated mutable state. Instances
 * of this structure are bound as the closure state as part of
 * function registration in the implementation of the primitive `object`.
 */
typedef struct {
    /** In-model function value. */
    zvalue function;

    /** Arbitrary state value. */
    zvalue state;

    /** Busy flag, used to prevent recursion. */
    bool busy;
} Object;

/**
 * The C function that is bound by the `object` primitive.
 */
static zvalue callObject(void *state, zint argCount, const zvalue *args) {
    Object *object = state;
    zvalue fullArgs[argCount + 1];

    if (object->busy) {
        die("Attempt to recursively call object.");
    }

    object->busy = true;

    fullArgs[0] = object->state;
    memcpy(fullArgs + 1, args, argCount);

    zvalue resultMaplet = langCall(object->function, argCount + 1, fullArgs);
    zvalue result;

    if (resultMaplet == NULL) {
        result = NULL;
    } else {
        zvalue newState = datMapletGet(resultMaplet, STR_STATE);
        if (newState != NULL) {
            object->state = newState;
        }

        result = datMapletGet(resultMaplet, STR_RESULT);
    }

    object->busy = false;
    return result;
}


/*
 * Exported primitives
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(apply) {
    requireAtLeast(argCount, 1);

    zvalue function = args[0];

    switch (argCount) {
        case 1: {
            // Zero-argument call.
            return langCall(function, 0, NULL);
        }
        case 2: {
            // Just a "rest" listlet.
            return langApply(function, args[1]);
        }
    }

    // The hard case: We make a flattened array of all the initial arguments
    // followed by the contents of the "rest" listlet.

    zvalue rest = args[argCount - 1];
    zint restSize = datSize(rest);

    args++;
    argCount -= 2;

    zint flatSize = argCount + restSize;
    zvalue flatArgs[flatSize];

    for (zint i = 0; i < argCount; i++) {
        flatArgs[i] = args[i];
    }

    datArrayFromListlet(flatArgs + argCount, rest);
    return langCall(function, flatSize, flatArgs);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(object) {
    requireExactly(argCount, 2);

    Object *object = zalloc(sizeof(Object));
    object->function = args[0];
    object->state = args[1];
    object->busy = false;

    return langDefineFunction(callObject, object);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Tree) {
    requireExactly(argCount, 1);
    return langNodeFromProgramText(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Eval) {
    requireExactly(argCount, 2);

    zvalue contextMaplet = args[0];
    zvalue expressionNode = args[1];
    zcontext ctx = langCtxNew();

    langCtxBindAll(ctx, contextMaplet);
    return langEvalExpressionNode(ctx, expressionNode);
}
