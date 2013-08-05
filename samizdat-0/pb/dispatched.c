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

    return datStringFromUtf8(-1, result);
}

/* Documented in header. */
static zvalue Default_eq(zvalue state, zint argCount, const zvalue *args) {
    return NULL;
}

/* Documented in header. */
static zvalue Default_sizeOf(zvalue state, zint argCount, const zvalue *args) {
    return datIntFromZint(0);
}

/* Documented in header. */
static zvalue Default_typeOf(zvalue state, zint argCount, const zvalue *args) {
    zvalue value = args[0];
    return datTypeFromZtype(value->type);
}

/* Documented in header. */
void pbInitCoreGenerics(void) {
    GFN_call = datGfnFrom(1, 1, datStringFromUtf8(-1, "call"));
    pbImmortalize(GFN_call);

    GFN_dataOf = datGfnFrom(1, 1, datStringFromUtf8(-1, "dataOf"));
    datGfnBindCoreDefault(GFN_dataOf, Default_dataOf);
    pbImmortalize(GFN_dataOf);

    GFN_debugString = datGfnFrom(1, 1, datStringFromUtf8(-1, "debugString"));
    datGfnBindCoreDefault(GFN_debugString, Default_debugString);
    pbImmortalize(GFN_debugString);

    GFN_eq = datGfnFrom(2, 2, datStringFromUtf8(-1, "eq"));
    datGfnBindCoreDefault(GFN_eq, Default_eq);
    pbImmortalize(GFN_eq);

    GFN_gcFree = datGfnFrom(1, 1, datStringFromUtf8(-1, "gcFree"));
    pbImmortalize(GFN_gcFree);

    GFN_gcMark = datGfnFrom(1, 1, datStringFromUtf8(-1, "gcMark"));
    pbImmortalize(GFN_gcMark);

    GFN_order = datGfnFrom(2, 2, datStringFromUtf8(-1, "order"));
    pbImmortalize(GFN_order);

    GFN_sizeOf = datGfnFrom(1, 1, datStringFromUtf8(-1, "sizeOf"));
    datGfnBindCoreDefault(GFN_sizeOf, Default_sizeOf);
    pbImmortalize(GFN_sizeOf);

    GFN_typeOf = datGfnFrom(1, 1, datStringFromUtf8(-1, "typeOf"));
    datGfnBindCoreDefault(GFN_typeOf, Default_typeOf);
    pbImmortalize(GFN_typeOf);
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datDataOf(zvalue value) {
    return datCall(GFN_dataOf, 1, &value);
}

/* Documented in header. */
char *datDebugString(zvalue value) {
    if (value == NULL) {
        return strdup("(null)");
    }

    zvalue result = datCall(GFN_debugString, 1, &value);
    zint size = datUtf8SizeFromString(result);
    char str[size + 1];

    datUtf8FromString(size + 1, str, result);
    return strdup(str);
}

/* Documented in header. */
bool datEq(zvalue v1, zvalue v2) {
    datAssertValid(v1);
    datAssertValid(v2);

    if (v1 == v2) {
        return true;
    } else if (v1->type != v2->type) {
        return false;
    } else {
        zvalue args[2] = { v1, v2 };
        return datCall(GFN_eq, 2, args) != NULL;
    }
}

/* Documented in header. */
zorder datOrder(zvalue v1, zvalue v2) {
    datAssertValid(v1);
    datAssertValid(v2);

    if (v1 == v2) {
        return ZSAME;
    } else if (v1->type == v2->type) {
        zvalue args[2] = { v1, v2 };
        zstackPointer save = pbFrameStart();
        zorder result = datZintFromInt(datCall(GFN_order, 2, args));
        pbFrameReturn(save, NULL);
        return result;
    } else if (datCoreTypeIs(v1, DAT_Deriv)) {
        // Per spec, derived values always sort after primitives.
        return ZMORE;
    } else {
        return (strcmp(v1->type->name, v2->type->name) < 0) ? ZLESS : ZMORE;
    }
}

/* Documented in header. */
zint datSize(zvalue value) {
    return datZintFromInt(datCall(GFN_sizeOf, 1, &value));
}

/* Documented in header. */
bool datTypeIs(zvalue value, zvalue type) {
    return datEq(datTypeOf(value), type);
}

/* Documented in header. */
zvalue datTypeOf(zvalue value) {
    return datCall(GFN_typeOf, 1, &value);
}
