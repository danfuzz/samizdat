/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "cst.h"
#include "io.h"
#include "impl.h"
#include "lib.h"
#include "util.h"

#include <stddef.h>


/*
 * Helper functions
 */

/**
 * Check the given argument count for an exact required amount,
 * complaining if it doesn't match.
 */
static void requireExactly(zint argCount, zint required) {
    if (argCount != required) {
        die("Invalid argument count for primitive: %lld != %lld",
            argCount, required);
    }
}

/**
 * Check the given argument count for a range of acceptable values,
 * complaining if it doesn't match.
 */
static void requireRange(zint argCount, zint min, zint max) {
    if (argCount < min) {
        die("Invalid argument count for primitive: %lld < %lld",
            argCount, min);
    } else if (argCount > max) {
        die("Invalid argument count for primitive: %lld > %lld",
            argCount, max);
    }
}


/*
 * Primitive implementations (exported via the context)
 */

/**
 * TODO: Document!
 */
static zvalue prim_lowOrder(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datOrder(args[0], args[1]));
}

/**
 * TODO: Document!
 */
static zvalue prim_lowOrderIs(void *state, zint argCount, const zvalue *args) {
    requireRange(argCount, 3, 4);

    zorder comp = datOrder(args[0], args[1]);
    bool result =
        (comp == datIntletToInt(args[2])) ||
        ((argCount == 4) && (comp == datIntletToInt(args[3])));

    return langBooleanFromBool(result);
}

/**
 * TODO: Document!
 */
static zvalue prim_lowSize(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return datIntletFromInt(datSize(args[0]));
}

/**
 * TODO: Document!
 */
static zvalue prim_lowType(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);

    switch (datType(args[0])) {
        case DAT_INTLET:  return CST_STR_INTLET;
        case DAT_LISTLET: return CST_STR_LISTLET;
        case DAT_MAPLET:  return CST_STR_MAPLET;
        case DAT_UNIQLET: return CST_STR_UNIQLET;
        default: {
            die("Invalid value type (shouldn't happen): %d", datType(args[0]));
        }
    }
}

/**
 * TODO: Document!
 */
static zvalue prim_not(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return langBooleanFromBool(!langBooleanToBool(args[0]));
}

/**
 * TODO: Document!
 */
static zvalue prim_ifElse(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 3);
    zvalue func = langIsTrue(args[0]) ? args[1] : args[2];
    return langCall(func, 0, NULL);
}

/**
 * TODO: Document!
 */
static zvalue prim_ineg(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return datIntletFromInt(-datIntletToInt(args[0]));
}

/**
 * TODO: Document!
 */
static zvalue prim_iadd(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) + datIntletToInt(args[1]));
}

/**
 * TODO: Document!
 */
static zvalue prim_isub(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) - datIntletToInt(args[1]));
}

/**
 * TODO: Document!
 */
static zvalue prim_imul(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) * datIntletToInt(args[1]));
}

/**
 * TODO: Document!
 */
static zvalue prim_idiv(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) / datIntletToInt(args[1]));
}

/**
 * TODO: Document!
 */
static zvalue prim_imod(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) % datIntletToInt(args[1]));
}

/**
 * TODO: Document!
 */
static zvalue prim_append(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datListletAppend(args[0], args[1]);
}

/**
 * TODO: Document!
 */
static zvalue prim_getNth(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);

    zvalue listlet = args[0];
    zint index = datIntletToInt(args[1]);

    return datListletGet(listlet, index);
}

/**
 * TODO: Document!
 */
static zvalue prim_delNth(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);

    zvalue listlet = args[0];
    zint index = datIntletToInt(args[1]);

    return datListletDelete(listlet, index);
}

/**
 * TODO: Document!
 */
static zvalue prim_getKeys(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return datMapletKeys(args[0]);
}

/**
 * TODO: Document!
 */
static zvalue prim_getValue(void *state, zint argCount, const zvalue *args) {
    requireRange(argCount, 2, 3);

    zvalue result = datMapletGet(args[0], args[1]);

    if (result == NULL) {
        if (argCount != 3) {
            die("Key not found in maplet.");
        } else {
            return args[3];
        }
    }

    return result;
}

/**
 * TODO: Document!
 */
static zvalue prim_apply(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return langApply(args[0], args[1]);
}

/**
 * TODO: Document!
 */
static zvalue prim_readFile(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return readFile(args[0]);
}

/**
 * TODO: Document!
 */
static zvalue prim_writeFile(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    writeFile(args[0], args[1]);
    return CST_NULL;
}


/*
 * Module functions
 */

/* Documented in header. */
void bindPrimitives(zcontext ctx) {
    // Low-layer types (in general)
    langBindFunction(ctx, "lowOrder",   prim_lowOrder,   NULL);
    langBindFunction(ctx, "lowOrderIs", prim_lowOrderIs, NULL);
    langBindFunction(ctx, "lowSize",    prim_lowSize,    NULL);
    langBindFunction(ctx, "lowType",    prim_lowType,    NULL);

    // Boolean(esque)
    langBindFunction(ctx, "not",    prim_not,    NULL);
    langBindFunction(ctx, "ifElse", prim_ifElse, NULL);

    // Intlets
    langBindFunction(ctx, "ineg", prim_ineg, NULL);
    langBindFunction(ctx, "iadd", prim_iadd, NULL);
    langBindFunction(ctx, "isub", prim_isub, NULL);
    langBindFunction(ctx, "imul", prim_imul, NULL);
    langBindFunction(ctx, "idiv", prim_idiv, NULL);
    langBindFunction(ctx, "imod", prim_imod, NULL);

    // Listlets
    langBindFunction(ctx, "append", prim_append, NULL);
    langBindFunction(ctx, "getNth", prim_getNth, NULL);
    langBindFunction(ctx, "delNth", prim_delNth, NULL);

    // Maplets
    langBindFunction(ctx, "getKeys",  prim_getKeys,  NULL);
    langBindFunction(ctx, "getValue", prim_getValue, NULL);

    // Functions
    langBindFunction(ctx, "apply", prim_apply, NULL);

    // I/O
    langBindFunction(ctx, "readFile",  prim_readFile,  NULL);
    langBindFunction(ctx, "writeFile", prim_writeFile, NULL);

    // TODO: More.
}
