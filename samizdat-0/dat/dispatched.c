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
zvalue genCall = NULL;

/* Documented in header. */
zvalue genDataOf = NULL;

/* Documented in header. */
zvalue genDebugString = NULL;

/* Documented in header. */
zvalue genEq = NULL;

/* Documented in header. */
zvalue genGcFree = NULL;

/* Documented in header. */
zvalue genGcMark = NULL;

/* Documented in header. */
zvalue genOrder = NULL;

/* Documented in header. */
zvalue genSizeOf = NULL;

/* Documented in header. */
zvalue genTypeOf = NULL;

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
void datInitCoreGenerics(void) {
    genCall = datGfnFrom(1, 1, datStringFromUtf8(-1, "call"));
    datImmortalize(genCall);

    genDataOf = datGfnFrom(1, 1, datStringFromUtf8(-1, "dataOf"));
    datGfnBindCoreDefault(genDataOf, Default_dataOf);
    datImmortalize(genDataOf);

    genDebugString = datGfnFrom(1, 1, datStringFromUtf8(-1, "debugString"));
    datGfnBindCoreDefault(genDebugString, Default_debugString);
    datImmortalize(genDebugString);

    genEq = datGfnFrom(2, 2, datStringFromUtf8(-1, "eq"));
    datGfnBindCoreDefault(genEq, Default_eq);
    datImmortalize(genEq);

    genGcFree = datGfnFrom(1, 1, datStringFromUtf8(-1, "gcFree"));
    datImmortalize(genGcFree);

    genGcMark = datGfnFrom(1, 1, datStringFromUtf8(-1, "gcMark"));
    datImmortalize(genGcMark);

    genOrder = datGfnFrom(2, 2, datStringFromUtf8(-1, "order"));
    datImmortalize(genOrder);

    genSizeOf = datGfnFrom(1, 1, datStringFromUtf8(-1, "sizeOf"));
    datGfnBindCoreDefault(genSizeOf, Default_sizeOf);
    datImmortalize(genSizeOf);

    genTypeOf = datGfnFrom(1, 1, datStringFromUtf8(-1, "typeOf"));
    datGfnBindCoreDefault(genTypeOf, Default_typeOf);
    datImmortalize(genTypeOf);
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datDataOf(zvalue value) {
    return datCall(genDataOf, 1, &value);
}

/* Documented in header. */
char *datDebugString(zvalue value) {
    if (value == NULL) {
        return strdup("(null)");
    }

    zvalue result = datCall(genDebugString, 1, &value);
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
        return datCall(genEq, 2, args) != NULL;
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
        zstackPointer save = datFrameStart();
        zorder result = datZintFromInt(datCall(genOrder, 2, args));
        datFrameReturn(save, NULL);
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
    return datZintFromInt(datCall(genSizeOf, 1, &value));
}

/* Documented in header. */
bool datTypeIs(zvalue value, zvalue type) {
    return datEq(datTypeOf(value), type);
}

/* Documented in header. */
zvalue datTypeOf(zvalue value) {
    return datCall(genTypeOf, 1, &value);
}
