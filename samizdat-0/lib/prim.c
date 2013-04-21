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
 * Primitive implementations (exported via the context)
 */

/*
 * Ultraprimitives (required in order to successfully execute parse
 * trees)
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeHighlet) {
    requireRange(argCount, 1, 2);

    zvalue value = (argCount == 2) ? args[1] : NULL;
    return datHighletFrom(args[0], value);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeListlet) {
    return datListletFromArray(argCount, args);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeMaplet) {
    requireEven(argCount);

    zvalue result = datMapletEmpty();

    for (zint i = 0; i < argCount; i += 2) {
        result = datMapletPut(result, args[i], args[i + 1]);
    }

    return result;
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(makeUniqlet) {
    requireExactly(argCount, 0);
    return datUniqlet();
}

/*
 * The rest (to be sorted)
 */

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowOrder) {
    requireExactly(argCount, 2);
    return datIntletFromInt(datOrder(args[0], args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowOrderIs) {
    requireRange(argCount, 3, 4);

    zorder comp = datOrder(args[0], args[1]);
    bool result =
        (comp == datIntFromIntlet(args[2])) ||
        ((argCount == 4) && (comp == datIntFromIntlet(args[3])));

    return langBooleanFromBool(result);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowSize) {
    requireExactly(argCount, 1);
    return datIntletFromInt(datSize(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(lowType) {
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
PRIM_IMPL(ifTrue) {
    requireRange(argCount, 2, 3);

    if (langBoolFromBoolean(langCall(args[0], 0, NULL))) {
        return langCall(args[1], 0, NULL);
    } else if (argCount ==3) {
        return langCall(args[2], 0, NULL);
    } else {
        return NULL;
    }
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(ifValue) {
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
PRIM_IMPL(ineg) {
    requireExactly(argCount, 1);
    return datIntletFromInt(-datIntFromIntlet(args[0]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(iadd) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) + datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(isub) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) - datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(imul) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) * datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(idiv) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) / datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(imod) {
    requireExactly(argCount, 2);
    return datIntletFromInt(
        datIntFromIntlet(args[0]) % datIntFromIntlet(args[1]));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletFromChar) {
    requireExactly(argCount, 1);
    return datStringletFromIntlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletFromChars) {
    requireExactly(argCount, 1);
    return datStringletFromListlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(charsFromStringlet) {
    requireExactly(argCount, 1);
    return datListletFromStringlet(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletAdd) {
    requireExactly(argCount, 2);
    return datStringletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(stringletNth) {
    requireExactly(argCount, 2);

    zvalue stringlet = args[0];
    zint index = datIntFromIntlet(args[1]);

    return datIntletFromInt(datStringletGet(stringlet, index));
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletAppend) {
    requireExactly(argCount, 2);
    return datListletAppend(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletPrepend) {
    requireExactly(argCount, 2);
    return datListletPrepend(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletAdd) {
    requireExactly(argCount, 2);
    return datListletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletNth) {
    requireExactly(argCount, 2);

    zvalue listlet = args[0];
    zint index = datIntFromIntlet(args[1]);

    return datListletGet(listlet, index);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(listletDelNth) {
    requireExactly(argCount, 2);

    zvalue listlet = args[0];
    zint index = datIntFromIntlet(args[1]);

    return datListletDelete(listlet, index);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletAdd) {
    requireExactly(argCount, 2);
    return datMapletAdd(args[0], args[1]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletKeys) {
    requireExactly(argCount, 1);
    return datMapletKeys(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(mapletGet) {
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
PRIM_IMPL(mapletPut) {
    requireExactly(argCount, 3);
    return datMapletPut(args[0], args[1], args[2]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(highletType) {
    requireExactly(argCount, 1);
    return datHighletType(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(highletValue) {
    requireExactly(argCount, 1);
    return datHighletValue(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(apply) {
    requireExactly(argCount, 2);
    return langApply(args[0], args[1]);
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

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(die) {
    requireRange(argCount, 0, 1);

    const char *message =
        (argCount == 0) ? "Alas" : datStringletEncodeUtf8(args[0], NULL);

    die("%s", message);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(readFile) {
    requireExactly(argCount, 1);
    return readFile(args[0]);
}

/* Documented in Samizdat Layer 0 spec. */
PRIM_IMPL(writeFile) {
    requireExactly(argCount, 2);
    writeFile(args[0], args[1]);
    return NULL;
}


/*
 * Module functions
 */

/** Documented in header. */
void requireExactly(zint argCount, zint required) {
    if (argCount != required) {
        die("Invalid argument count for primitive: %lld != %lld",
            argCount, required);
    }
}

/** Documented in header. */
void requireRange(zint argCount, zint min, zint max) {
    if (argCount < min) {
        die("Invalid argument count for primitive: %lld < %lld",
            argCount, min);
    } else if (argCount > max) {
        die("Invalid argument count for primitive: %lld > %lld",
            argCount, max);
    }
}

/** Documented in header. */
void requireEven(zint argCount) {
    if ((argCount & 0x01) != 0) {
        die("Invalid non-even argument count for primitive: %lld", argCount);
    }
}

/* Documented in header. */
zcontext primitiveContext(void) {
    cstInit();

    zcontext ctx = langCtxNew();

    // These all could have been defined in-language, but we already
    // have to make them be defined and accessible to C code, so we just
    // go ahead and bind them here.
    langCtxBind(ctx, "false", CST_FALSE);
    langCtxBind(ctx, "true", CST_TRUE);

    // Bind all the primitive functions.
    #define PRIM_FUNC(name) langCtxBindFunction(ctx, #name, prim_##name, NULL)
    #include "prim-def.h"

    // Include a binding for a maplet of all the bindings.
    langCtxBind(ctx, "PRIMLIB", langMapletFromCtx(ctx));

    return ctx;
}
