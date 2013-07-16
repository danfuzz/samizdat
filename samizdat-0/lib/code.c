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
 * Object helper definitions
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
    utilFree(state);
}

/** Uniqlet dispatch table for objects. */
static DatUniqletDispatch OBJECT_DISPATCH = {
    objectMark,
    objectFree
};

/**
 * The C function that is the bound in the direct result of an
 * `object` primitive.
 */
static zvalue callObject(zvalue state, zint argCount, const zvalue *args) {
    Object *object = datUniqletGetState(state, &OBJECT_DISPATCH);
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

    Object *object = datUniqletGetState(state, &OBJECT_DISPATCH);

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
PRIM_IMPL(boxGet) {
    requireRange(argCount, 1, 2);

    zvalue result = boxGet(args[0]);

    if ((result == NULL) && (argCount == 2)) {
        return args[1];
    } else {
        return result;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(boxIsSet) {
    requireExactly(argCount, 1);

    zvalue box = args[0];

    return boxIsSet(box) ? box : NULL;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(boxSet) {
    requireRange(argCount, 1, 2);

    zvalue result = (argCount == 2) ? args[1] : NULL;

    boxSet(args[0], result);
    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mutableBox) {
    requireRange(argCount, 0, 1);

    zvalue result = boxMutable();

    if (argCount == 1) {
        boxSet(result, args[0]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(nonlocalExit) {
    requireRange(argCount, 1, 2);

    zvalue yieldFunction = args[0];
    zvalue value;

    if (argCount == 1) {
        value = NULL;
    } else {
        value = langCall(args[1], 0, NULL);
    }

    if (value == NULL) {
        langCall(yieldFunction, 0, NULL);
    } else {
        langCall(yieldFunction, 1, &value);
    }

    die("Nonlocal exit function did not perform nonlocal exit.");
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(object) {
    requireExactly(argCount, 2);

    Object *object = utilAlloc(sizeof(Object));
    zvalue objectUniqlet = datUniqletWith(&OBJECT_DISPATCH, object);

    object->serviceFunction = args[0];
    object->yieldFunction = langDefineFunction(callYield, objectUniqlet, NULL);
    object->state = args[1];
    object->busy = false;
    object->yieldValue = NULL;
    object->yielded = false;

    return langDefineFunction(callObject, objectUniqlet, NULL);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Tokenize) {
    requireExactly(argCount, 1);
    return langTokenize0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Tree) {
    requireExactly(argCount, 1);
    return langTree0(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(sam0Eval) {
    requireExactly(argCount, 2);

    zvalue ctx = args[0];
    zvalue expressionNode = args[1];

    return langEval0(ctx, expressionNode);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(yieldBox) {
    return boxYield();
}
