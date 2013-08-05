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
zvalue GFN_dataOf = NULL;

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
zvalue GFN_sizeOf = NULL;

/* Documented in header. */
zvalue GFN_typeOf = NULL;

/* Documented in header. */
static zvalue Default_dataOf(zvalue state, zint argCount, const zvalue *args) {
    return args[0];
}

/* Documented in header. */
static zvalue Default_debugString(zvalue state,
        zint argCount, const zvalue *args) {
    char *result = NULL;

    asprintf(&result, "@(%s @ %p)", args[0]->type->name, args[0]);
    if (result == NULL) {
        die("Trouble with `asprintf` (shouldn't happen).");
    }

    return stringFromUtf8(-1, result);
}

/* Documented in header. */
static zvalue Default_eq(zvalue state, zint argCount, const zvalue *args) {
    return NULL;
}

/* Documented in header. */
static zvalue Default_sizeOf(zvalue state, zint argCount, const zvalue *args) {
    return intFromZint(0);
}

/* Documented in header. */
static zvalue Default_typeOf(zvalue state, zint argCount, const zvalue *args) {
    zvalue value = args[0];
    return datTypeFromZtype(value->type);
}

/* Documented in header. */
void pbInitCoreGenerics(void) {
    GFN_call = gfnFrom(1, 1, stringFromUtf8(-1, "call"));
    pbImmortalize(GFN_call);

    GFN_dataOf = gfnFrom(1, 1, stringFromUtf8(-1, "dataOf"));
    gfnBindCoreDefault(GFN_dataOf, Default_dataOf);
    pbImmortalize(GFN_dataOf);

    GFN_debugString = gfnFrom(1, 1, stringFromUtf8(-1, "debugString"));
    gfnBindCoreDefault(GFN_debugString, Default_debugString);
    pbImmortalize(GFN_debugString);

    GFN_eq = gfnFrom(2, 2, stringFromUtf8(-1, "eq"));
    gfnBindCoreDefault(GFN_eq, Default_eq);
    pbImmortalize(GFN_eq);

    GFN_gcFree = gfnFrom(1, 1, stringFromUtf8(-1, "gcFree"));
    pbImmortalize(GFN_gcFree);

    GFN_gcMark = gfnFrom(1, 1, stringFromUtf8(-1, "gcMark"));
    pbImmortalize(GFN_gcMark);

    GFN_order = gfnFrom(2, 2, stringFromUtf8(-1, "order"));
    pbImmortalize(GFN_order);

    GFN_sizeOf = gfnFrom(1, 1, stringFromUtf8(-1, "sizeOf"));
    gfnBindCoreDefault(GFN_sizeOf, Default_sizeOf);
    pbImmortalize(GFN_sizeOf);

    GFN_typeOf = gfnFrom(1, 1, stringFromUtf8(-1, "typeOf"));
    gfnBindCoreDefault(GFN_typeOf, Default_typeOf);
    pbImmortalize(GFN_typeOf);
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue pbDataOf(zvalue value) {
    return fnCall(GFN_dataOf, 1, &value);
}

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
    pbAssertValid(v1);
    pbAssertValid(v2);

    if (v1 == v2) {
        return true;
    } else if (v1->type != v2->type) {
        return false;
    } else {
        zvalue args[2] = { v1, v2 };
        return fnCall(GFN_eq, 2, args) != NULL;
    }
}

/* Documented in header. */
zorder pbOrder(zvalue v1, zvalue v2) {
    pbAssertValid(v1);
    pbAssertValid(v2);

    if (v1 == v2) {
        return ZSAME;
    } else if (v1->type == v2->type) {
        zvalue args[2] = { v1, v2 };
        zstackPointer save = pbFrameStart();
        zorder result = zintFromInt(fnCall(GFN_order, 2, args));
        pbFrameReturn(save, NULL);
        return result;
    } else if (pbCoreTypeIs(v1, DAT_Deriv)) {
        // Per spec, derived values always sort after primitives.
        return ZMORE;
    } else {
        return (strcmp(v1->type->name, v2->type->name) < 0) ? ZLESS : ZMORE;
    }
}

/* Documented in header. */
zint pbSize(zvalue value) {
    return zintFromInt(fnCall(GFN_sizeOf, 1, &value));
}

/* Documented in header. */
bool pbTypeIs(zvalue value, zvalue type) {
    return pbEq(pbTypeOf(value), type);
}

/* Documented in header. */
zvalue pbTypeOf(zvalue value) {
    return fnCall(GFN_typeOf, 1, &value);
}
