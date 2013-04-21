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
