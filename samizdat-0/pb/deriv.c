/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <stddef.h>


/*
 * Helper definitions
 */

/**
 * Derived value structure.
 */
typedef struct {
    /** Type tag. Never `NULL`. */
    zvalue type;

    /** Associated payload data. Possibly `NULL`. */
    zvalue data;
} DerivInfo;

/**
 * Gets a pointer to the value's info.
 */
static DerivInfo *derivInfo(zvalue deriv) {
    return pbPayload(deriv);
}

/**
 * Allocates and initializes a derived value, without doing error-checking
 * on the arguments.
 */
static zvalue newDeriv(zvalue type, zvalue data) {
    zvalue result = pbAllocValue(TYPE_Deriv, sizeof(DerivInfo));
    DerivInfo *info = derivInfo(result);

    info->type = type;
    info->data = data;
    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
zvalue Transparent_dataOf(zvalue state, zint argCount, const zvalue *args) {
    zvalue value = args[0];
    return ((TransparentInfo *) pbPayload(value))->data;
}

/* Documented in header. */
zvalue Transparent_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue value = args[0];
    pbMark(((TransparentInfo *) pbPayload(value))->data);
    return NULL;
}


/*
 * Exported functions
 */

// TODO: This is now misnamed. It makes general transparent values.
zvalue derivFrom(zvalue type, zvalue data) {
    pbAssertValid(type);

    if (pbTypeOf(type) == TYPE_Type) {
        // This should probably be fixed.
        die("Can't create transparent value given type value.");
    }

    if (data != NULL) {
        pbAssertValid(data);
    }

    // Get a `Type` per se. `NULL` indicates it is to be transparent.
    type = typeFrom(type, NULL);
    zvalue result = pbAllocValue(type, sizeof(TransparentInfo));
    ((TransparentInfo *) pbPayload(result))->data = data;

    return result;
}


/*
 * Type binding
 */

/* Documented in header. */
static zvalue Deriv_dataOf(zvalue state, zint argCount, const zvalue *args) {
    zvalue deriv = args[0];
    return derivInfo(deriv)->data;
}

/* Documented in header. */
static zvalue Deriv_eq(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    DerivInfo *info1 = derivInfo(v1);
    DerivInfo *info2 = derivInfo(v2);

    return (pbNullSafeEq(info1->data, info2->data) &&
        pbEq(info1->type, info2->type))
        ? v2 : NULL;
}

/* Documented in header. */
static zvalue Deriv_gcMark(zvalue state, zint argCount, const zvalue *args) {
    zvalue deriv = args[0];
    DerivInfo *info = derivInfo(deriv);

    pbMark(info->type);
    pbMark(info->data);

    return NULL;
}

/* Documented in header. */
static zvalue Deriv_order(zvalue state, zint argCount, const zvalue *args) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    DerivInfo *info1 = derivInfo(v1);
    DerivInfo *info2 = derivInfo(v2);

    zorder result = pbOrder(info1->type, info2->type);

    if (result != ZSAME) {
        return intFromZint(result);
    } else if (info1->data == NULL) {
        return (info2->data == NULL) ? PB_0 : PB_NEG1;
    } else if (info2->data == NULL) {
        return PB_1;
    } else {
        return intFromZint(pbOrder(info1->data, info2->data));
    }
}

/* Documented in header. */
static zvalue Deriv_sizeOf(zvalue state, zint argCount, const zvalue *args) {
    zvalue deriv = args[0];
    return (derivInfo(deriv)->data == NULL) ? PB_0 : PB_1;
}

/* Documented in header. */
void pbBindDeriv(void) {
    TYPE_Deriv = typeFrom(stringFromUtf8(-1, "Deriv"), PB_SECRET);
    gfnBindCore(GFN_dataOf, TYPE_Deriv, Deriv_dataOf);
    gfnBindCore(GFN_eq,     TYPE_Deriv, Deriv_eq);
    gfnBindCore(GFN_gcMark, TYPE_Deriv, Deriv_gcMark);
    gfnBindCore(GFN_order,  TYPE_Deriv, Deriv_order);
    gfnBindCore(GFN_sizeOf, TYPE_Deriv, Deriv_sizeOf);
}

/* Documented in header. */
zvalue TYPE_Deriv = NULL;
