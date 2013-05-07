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
    /** In-model service function. */
    zvalue serviceFunction;

    /** In-model yield (return result) function. */
    zvalue yieldFunction;

    /** Arbitrary state value. */
    zvalue state;

    /** Busy flag, used to prevent recursion. */
    bool busy;

    /** Most recently yielded value. */
    zvalue yieldValue;

    /** Yielded flag, used to prevent multi-yield per message. */
    bool yielded;
} Object;

/**
 * Marks an object state for garbage collection.
 */
static void objectMark(void *state) {
    Object *object = state;

    datMark(object->serviceFunction);
    datMark(object->yieldFunction);
    datMark(object->state);
    datMark(object->yieldValue);
}

/**
 * Frees an object state.
 */
static void objectFree(void *state) {
    zfree(state);
}

/** Uniqlet dispatch table. */
static DatUniqletDispatch OBJECT_DISPATCH = {
    objectMark,
    objectFree
};

/**
 * The C function that is the bound in the direct result of an
 * `object` primitive.
 */
static zvalue callObject(zvalue state, zint argCount, const zvalue *args) {
    Object *object = datUniqletGetValue(state, &OBJECT_DISPATCH);
    zvalue fullArgs[argCount + 2];

    if (object->busy) {
        die("Attempt to recursively call object.");
    }

    object->busy = true;
    object->yieldValue = NULL;
    object->yielded = false;

    fullArgs[0] = object->yieldFunction;
    fullArgs[1] = object->state;
    memcpy(fullArgs + 2, args, argCount * sizeof(zvalue));

    zvalue newState = langCall(object->serviceFunction, argCount + 2, fullArgs);

    if (newState != NULL) {
        object->state = newState;
    }

    if (!object->yielded) {
        die("Failure to yield from object.");
    }

    zvalue result = object->yieldValue;

    object->busy = false;
    object->yieldValue = NULL;
    object->yielded = false;

    return result;
}

/**
 * The C function that is bound into yield functions.
 */
static zvalue callYield(zvalue state, zint argCount, const zvalue *args) {
    requireRange(argCount, 0, 1);

    Object *object = datUniqletGetValue(state, &OBJECT_DISPATCH);

    if (!object->busy) {
        die("Attempt to yield from inactive object.");
    } else if (object->yielded) {
        die("Attempt to double-yield from object.");
    }

    if (argCount == 1) {
        object->yieldValue = args[0];
    }

    object->yielded = true;
    return NULL;
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
    zvalue objectUniqlet = datUniqletWith(&OBJECT_DISPATCH, object);

    object->serviceFunction = args[0];
    object->yieldFunction = langDefineFunction(callYield, objectUniqlet);
    object->state = args[1];
    object->busy = false;
    object->yieldValue = NULL;
    object->yielded = false;

    return langDefineFunction(callObject, objectUniqlet);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Tree) {
    requireExactly(argCount, 1);
    return langNodeFromProgramText(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Eval) {
    requireExactly(argCount, 2);

    zvalue ctx = args[0];
    zvalue expressionNode = args[1];

    return langEvalExpressionNode(ctx, expressionNode);
}
