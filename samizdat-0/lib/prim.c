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

/**
 * Helper for all of the comparison functions, which parameterizes
 * which comparison to accept.
 */
static zvalue compare(zcomparison wanted, zint argCount, const zvalue *args) {
    for (zint i = 1; i < argCount; i ++) {
        if (datCompare(args[0], args[i]) != wanted) {
            return CST_FALSE;
        }
    }

    return CST_TRUE;
}

/**
 * Underlying implementation of `not`.
 */
static zvalue booleanNot(zvalue value) {
    if (datCompare(value, CST_FALSE) == 0) {
        return CST_TRUE;
    } else if (datCompare(value, CST_TRUE) == 0) {
        return CST_FALSE;
    } else {
        die("Non-boolean argument to not.");
    }
}


/*
 * Primitive implementations (exported via the context)
 */

/**
 * TODO: Document!
 */
static zvalue prim_eq(void *state, zint argCount, const zvalue *args) {
    if (argCount < 2) return CST_TRUE;
    return compare(ZEQUAL, argCount, args);
}

/**
 * TODO: Document!
 */
static zvalue prim_ne(void *state, zint argCount, const zvalue *args) {
    if (argCount < 2) return CST_TRUE;
    return booleanNot(compare(ZEQUAL, argCount, args));
}

/**
 * TODO: Document!
 */
static zvalue prim_lt(void *state, zint argCount, const zvalue *args) {
    if (argCount < 2) return CST_TRUE;
    return compare(ZLESS, argCount, args);
}

/**
 * TODO: Document!
 */
static zvalue prim_gt(void *state, zint argCount, const zvalue *args) {
    if (argCount < 2) return CST_TRUE;
    return compare(ZMORE, argCount, args);
}

/**
 * TODO: Document!
 */
static zvalue prim_le(void *state, zint argCount, const zvalue *args) {
    if (argCount < 2) return CST_TRUE;
    return booleanNot(compare(ZMORE, argCount, args));
}

/**
 * TODO: Document!
 */
static zvalue prim_ge(void *state, zint argCount, const zvalue *args) {
    if (argCount < 2) return CST_TRUE;
    return booleanNot(compare(ZLESS, argCount, args));
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
static zvalue prim_lowSize(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return datIntletFromInt(datSize(args[0]));
}

/**
 * TODO: Document!
 */
static zvalue prim_not(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return booleanNot(args[0]);
}

/**
 * TODO: Document!
 */
static zvalue prim_and(void *state, zint argCount, const zvalue *args) {
    for (zint i = 0; i < argCount; i++) {
        zvalue test = langCall(args[i], 0, NULL);
        if (datCompare(test, CST_FALSE) == 0) {
            return CST_FALSE;
        }
    }

    return CST_TRUE;
}

/**
 * TODO: Document!
 */
static zvalue prim_or(void *state, zint argCount, const zvalue *args) {
    for (zint i = 0; i < argCount; i++) {
        zvalue test = langCall(args[i], 0, NULL);
        if (datCompare(test, CST_FALSE) != 0) {
            return CST_TRUE;
        }
    }

    return CST_FALSE;
}

/**
 * TODO: Document!
 */
static zvalue prim_if(void *state, zint argCount, const zvalue *args) {
    if ((argCount % 2) != 0) {
        die("Invalid argument count to if: %lld", argCount);
    }

    for (zint i = 0; i < argCount; i += 2) {
        zvalue test = langCall(args[i], 0, NULL);
        if (datCompare(test, CST_FALSE) != 0) {
            return langCall(args[i + 1], 0, NULL);
        }
    }

    return CST_NULL;
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
static zvalue prim_cat(void *state, zint argCount, const zvalue *args) {
    if (argCount == 0) {
        return datListletEmpty();
    }

    zvalue result = args[0];
    for (zint i = 1; i < argCount; i ++) {
        zvalue one = args[i];
        zint size = datSize(one);
        datAssertListlet(one);
        for (zint j = 0; j < size; j++) {
            result = datListletAppend(result, datListletGet(one, j));
        }
    }

    return result;
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
    // Comparison
    langBindFunction(ctx, "eq", prim_eq, NULL);
    langBindFunction(ctx, "ne", prim_ne, NULL);
    langBindFunction(ctx, "lt", prim_lt, NULL);
    langBindFunction(ctx, "gt", prim_gt, NULL);
    langBindFunction(ctx, "le", prim_le, NULL);
    langBindFunction(ctx, "ge", prim_ge, NULL);

    // Boolean(esque)
    langBindFunction(ctx, "not", prim_not, NULL);
    langBindFunction(ctx, "and", prim_and, NULL);
    langBindFunction(ctx, "or",  prim_or, NULL);
    langBindFunction(ctx, "if",  prim_if,  NULL);

    // Intlets
    langBindFunction(ctx, "ineg", prim_ineg, NULL);
    langBindFunction(ctx, "iadd", prim_iadd, NULL);
    langBindFunction(ctx, "isub", prim_isub, NULL);
    langBindFunction(ctx, "imul", prim_imul, NULL);
    langBindFunction(ctx, "idiv", prim_idiv, NULL);
    langBindFunction(ctx, "imod", prim_imod, NULL);

    // Listlets
    langBindFunction(ctx, "cat",    prim_cat,    NULL);
    langBindFunction(ctx, "getNth", prim_getNth, NULL);
    langBindFunction(ctx, "delNth", prim_delNth, NULL);

    // Maplets
    langBindFunction(ctx, "getKeys",  prim_getKeys,  NULL);
    langBindFunction(ctx, "getValue", prim_getValue, NULL);

    // Data types
    langBindFunction(ctx, "lowSize", prim_lowSize, NULL);
    langBindFunction(ctx, "lowType", prim_lowType, NULL);

    // Other
    langBindFunction(ctx, "readFile",  prim_readFile,  NULL);
    langBindFunction(ctx, "writeFile", prim_writeFile, NULL);

    // TODO: More.
}
