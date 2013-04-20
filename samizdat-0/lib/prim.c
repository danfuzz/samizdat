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
 * Check that the given argument count is even, complaining if not.
 */
static void requireEven(zint argCount) {
    if ((argCount & 0x01) != 0) {
        die("Invalid non-even argument count for primitive: %lld", argCount);
    }
}


/*
 * Primitive implementations (exported via the context)
 */

/*
 * Ultraprimitives (required in order to successfully execute parse
 * trees)
 */

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_makeHighlet(void *state, zint argCount,
                               const zvalue *args) {
    requireRange(argCount, 1, 2);

    zvalue value = (argCount == 2) ? args[1] : NULL;
    return datHighletFrom(args[0], value);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_makeListlet(void *state, zint argCount,
                               const zvalue *args) {
    return datListletFromValues(argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_makeMaplet(void *state, zint argCount,
                              const zvalue *args) {
    requireEven(argCount);

    zvalue result = datMapletEmpty();

    for (zint i = 0; i < argCount; i += 2) {
        result = datMapletPut(result, args[i], args[i + 1]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_makeUniqlet(void *state, zint argCount,
                               const zvalue *args) {
    requireExactly(argCount, 0);
    return datUniqlet();
}

/*
 * The rest (to be sorted)
 */

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_lowOrder(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datOrder(args[0], args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_lowOrderIs(void *state, zint argCount, const zvalue *args) {
    requireRange(argCount, 3, 4);

    zorder comp = datOrder(args[0], args[1]);
    bool result =
        (comp == datIntletToInt(args[2])) ||
        ((argCount == 4) && (comp == datIntletToInt(args[3])));

    return langBooleanFromBool(result);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_lowSize(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return datIntletFromInt(datSize(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_lowType(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);

    switch (datType(args[0])) {
        case DAT_INTLET:    return CST_STR_INTLET;
        case DAT_STRINGLET: return CST_STR_STRINGLET;
        case DAT_LISTLET:   return CST_STR_LISTLET;
        case DAT_MAPLET:    return CST_STR_MAPLET;
        case DAT_UNIQLET:   return CST_STR_UNIQLET;
        case DAT_HIGHLET:   return CST_STR_HIGHLET;
        default: {
            die("Invalid value type (shouldn't happen): %d", datType(args[0]));
        }
    }
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_ifTrue(void *state, zint argCount, const zvalue *args) {
    requireRange(argCount, 2, 3);

    if (langBooleanToBool(langCall(args[0], 0, NULL))) {
        return langCall(args[1], 0, NULL);
    } else if (argCount ==3) {
        return langCall(args[2], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_ifValue(void *state, zint argCount, const zvalue *args) {
    requireRange(argCount, 2, 3);

    zvalue result = langCall(args[0], 0, NULL);

    if (result != NULL) {
        return langCall(args[1], 1, &result);
    } else if (argCount == 3) {
        return langCall(args[2], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_ineg(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return datIntletFromInt(-datIntletToInt(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_iadd(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) + datIntletToInt(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_isub(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) - datIntletToInt(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_imul(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) * datIntletToInt(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_idiv(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) / datIntletToInt(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_imod(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datIntletToInt(args[0]) % datIntletToInt(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_stringletFromChar(void *state, zint argCount,
                                     const zvalue *args) {
    requireExactly(argCount, 1);
    return datStringletFromIntlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_stringletFromChars(void *state, zint argCount,
                                      const zvalue *args) {
    requireExactly(argCount, 1);
    return datStringletFromListlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_stringletToChars(void *state, zint argCount,
                                    const zvalue *args) {
    requireExactly(argCount, 1);
    return datStringletToListlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_stringletAdd(void *state, zint argCount,
                                const zvalue *args) {
    requireExactly(argCount, 2);
    return datStringletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_stringletNth(void *state, zint argCount,
                                const zvalue *args) {
    requireExactly(argCount, 2);

    zvalue stringlet = args[0];
    zint index = datIntletToInt(args[1]);

    return datIntletFromInt(datStringletGet(stringlet, index));
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_listletAppend(void *state, zint argCount,
                                 const zvalue *args) {
    requireExactly(argCount, 2);
    return datListletAppend(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_listletPrepend(void *state, zint argCount,
                                  const zvalue *args) {
    requireExactly(argCount, 2);
    return datListletPrepend(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_listletAdd(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datListletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_listletNth(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);

    zvalue listlet = args[0];
    zint index = datIntletToInt(args[1]);

    return datListletGet(listlet, index);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_listletDelNth(void *state, zint argCount,
                                 const zvalue *args) {
    requireExactly(argCount, 2);

    zvalue listlet = args[0];
    zint index = datIntletToInt(args[1]);

    return datListletDelete(listlet, index);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_mapletAdd(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return datMapletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_mapletKeys(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return datMapletKeys(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_mapletGet(void *state, zint argCount, const zvalue *args) {
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

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_highletType(void *state, zint argCount,
                               const zvalue *args) {
    requireExactly(argCount, 1);
    return datHighletType(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_highletValue(void *state, zint argCount,
                                const zvalue *args) {
    requireExactly(argCount, 1);
    return datHighletValue(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_apply(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    return langApply(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_sam0Tree(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return langNodeFromProgramText(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_sam0Function(void *state, zint argCount,
                                const zvalue *args) {
    requireExactly(argCount, 2);

    zvalue contextMaplet = args[0];
    zvalue functionNode = args[1];
    zcontext ctx = langCtxNew();

    langCtxBindAll(ctx, contextMaplet);
    return langFunctionFromNode(ctx, functionNode);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_die(void *state, zint argCount, const zvalue *args) {
    requireRange(argCount, 0, 1);

    const char *message =
        (argCount == 0) ? "Alas" : datStringletEncodeUtf8(args[0], NULL);

    die("%s", message);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_readFile(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 1);
    return readFile(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
static zvalue prim_writeFile(void *state, zint argCount, const zvalue *args) {
    requireExactly(argCount, 2);
    writeFile(args[0], args[1]);
    return NULL;
}


/*
 * Module functions
 */

/* Documented in header. */
zcontext primitiveContext(void) {
    cstInit();

    zcontext ctx = langCtxNew();

    // These all could have been defined in-language, but we already
    // have to make them be defined and accessible to C code, so we just
    // go ahead and bind them here.
    langCtxBind(ctx, "false", CST_FALSE);
    langCtxBind(ctx, "true", CST_TRUE);

    // This avoids boilerplate below.
    #define BIND(name) langCtxBindFunction(ctx, #name, prim_##name, NULL)

    // Ultraprimitives
    BIND(makeHighlet);
    BIND(makeListlet);
    BIND(makeMaplet);
    BIND(makeUniqlet);

    // Low-layer types (in general)
    BIND(lowOrder);
    BIND(lowOrderIs);
    BIND(lowSize);
    BIND(lowType);

    // Conditional
    BIND(ifTrue);
    BIND(ifValue);

    // Intlets
    BIND(iadd);
    BIND(idiv);
    BIND(imod);
    BIND(imul);
    BIND(ineg);
    BIND(isub);

    // Stringlets
    BIND(stringletAdd);
    BIND(stringletFromChar);
    BIND(stringletFromChars);
    BIND(stringletNth);
    BIND(stringletToChars);

    // Listlets
    BIND(listletAdd);
    BIND(listletAppend);
    BIND(listletDelNth);
    BIND(listletNth);
    BIND(listletPrepend);

    // Maplets
    BIND(mapletAdd);
    BIND(mapletGet);
    BIND(mapletKeys);

    // Highlets
    BIND(highletType);
    BIND(highletValue);

    // Functions and Code
    BIND(apply);
    BIND(sam0Function);
    BIND(sam0Tree);

    // Miscellaneous
    BIND(die);
    BIND(readFile);
    BIND(writeFile);

    // Include a binding for a maplet of all the bindings.
    langCtxBind(ctx, "PRIMLIB", langCtxToMaplet(ctx));

    return ctx;
}
