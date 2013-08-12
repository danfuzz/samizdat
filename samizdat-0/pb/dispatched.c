/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>


/*
 * Module definitions
 */

/* Documented in header. */
zvalue GFN_call = NULL;

/* Documented in header. */
zvalue GFN_canCall = NULL;

/* Documented in header. */
zvalue GFN_debugString = NULL;

/* Documented in header. */
zvalue GFN_eq = NULL;

/* Documented in header. */
zvalue GFN_gcFree = NULL;

/* Documented in header. */
zvalue GFN_gcMark = NULL;

/* Documented in header. */
zvalue GFN_order = NULL;

/* Documented in header. */
zvalue GFN_size = NULL;

/* Documented in header. */
void pbInitCoreGenerics(void) {
    GFN_call = gfnFrom(1, -1, stringFromUtf8(-1, "call"));
    pbImmortalize(GFN_call);

    GFN_canCall = gfnFrom(1, -1, stringFromUtf8(-1, "canCall"));
    pbImmortalize(GFN_canCall);

    GFN_debugString = gfnFrom(1, 1, stringFromUtf8(-1, "debugString"));
    pbImmortalize(GFN_debugString);

    GFN_eq = gfnFrom(2, 2, stringFromUtf8(-1, "eq"));
    pbImmortalize(GFN_eq);

    GFN_gcFree = gfnFrom(1, 1, stringFromUtf8(-1, "gcFree"));
    pbImmortalize(GFN_gcFree);

    GFN_gcMark = gfnFrom(1, 1, stringFromUtf8(-1, "gcMark"));
    pbImmortalize(GFN_gcMark);

    GFN_order = gfnFrom(2, 2, stringFromUtf8(-1, "order"));
    pbImmortalize(GFN_order);

    GFN_size = gfnFrom(1, 1, stringFromUtf8(-1, "size"));
    pbImmortalize(GFN_size);
}


/*
 * Exported functions
 */

/* Documented in header. */
char *pbDebugString(zvalue value) {
    if (value == NULL) {
        return strdup("(null)");
    }

    zvalue result = fnCall(GFN_debugString, 1, &value);
    zint size = utf8SizeFromString(result);
    char str[size + 1];

    utf8FromString(size + 1, str, result);
    return strdup(str);
}

/* Documented in header. */
bool pbEq(zvalue v1, zvalue v2) {
    if (v1 == v2) {
        return true;
    } else if ((v1 == NULL) || (v2 == NULL)) {
        return false;
    }

    pbAssertValid(v1);
    pbAssertValid(v2);

    if (haveSameType(v1, v2)) {
        zvalue args[2] = { v1, v2 };
        return (fnCall(GFN_eq, 2, args) != NULL);
    } else {
        return false;
    }
}

/* Documented in header. */
zorder pbOrder(zvalue v1, zvalue v2) {
    if (v1 == v2) {
        return ZSAME;
    } else if (v1 == NULL) {
        return ZLESS;
    } else if (v2 == NULL) {
        return ZMORE;
    }

    pbAssertValid(v1);
    pbAssertValid(v2);

    if (haveSameType(v1, v2)) {
        zvalue args[2] = { v1, v2 };
        zstackPointer save = pbFrameStart();
        zorder result = zintFromInt(fnCall(GFN_order, 2, args));
        pbFrameReturn(save, NULL);
        return result;
    } else {
        return pbOrder(typeOf(v1), typeOf(v2));
    }
}

/* Documented in header. */
zint pbSize(zvalue value) {
    return zintFromInt(fnCall(GFN_size, 1, &value));
}
