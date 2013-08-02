/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>
#include <string.h>


/*
 * Module definitions
 */

/* Documented in header. */
zvalue genDataOf = NULL;

/* Documented in header. */
zvalue genSizeOf = NULL;

/* Documented in header. */
zvalue genTypeOf = NULL;

/* Documented in header. */
static zvalue Default_dataOf(zvalue state, zint argCount, const zvalue *args) {
    return args[0];
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
    genDataOf = datGenFrom(1, 1, datStringFromUtf8(-1, "dataOf"));
    datGenBindCoreDefault(genDataOf, Default_dataOf, NULL);
    datImmortalize(genDataOf);

    genSizeOf = datGenFrom(1, 1, datStringFromUtf8(-1, "sizeOf"));
    datGenBindCoreDefault(genSizeOf, Default_sizeOf, NULL);
    datImmortalize(genSizeOf);

    genTypeOf = datGenFrom(1, 1, datStringFromUtf8(-1, "typeOf"));
    datGenBindCoreDefault(genTypeOf, Default_typeOf, NULL);
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
bool datEq(zvalue v1, zvalue v2) {
    datAssertValid(v1);
    datAssertValid(v2);

    if (v1 == v2) {
        return true;
    } else if (v1->type != v2->type) {
        return false;
    } else if (v1->type->eq) {
        return v1->type->eq(v1, v2);
    } else {
        return false;
    }
}

/* Documented in header. */
zorder datOrder(zvalue v1, zvalue v2) {
    datAssertValid(v1);
    datAssertValid(v2);

    if (v1 == v2) {
        return ZSAME;
    } else if (v1->type == v2->type) {
        return v1->type->order(v1, v2);
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
