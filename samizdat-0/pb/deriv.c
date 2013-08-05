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
    zvalue result = pbAllocValue(PB_Deriv, sizeof(DerivInfo));
    DerivInfo *info = derivInfo(result);

    info->type = type;
    info->data = data;
    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
void pbAssertDeriv(zvalue value) {
    pbAssertType(value, PB_Deriv);
}

/* Documented in header. */
zvalue derivFrom(zvalue type, zvalue data) {
    pbAssertValid(type);

    if (data != NULL) {
        pbAssertValid(data);
    }

    return newDeriv(type, data);
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
static zvalue Deriv_typeOf(zvalue state, zint argCount, const zvalue *args) {
    zvalue deriv = args[0];
    return derivInfo(deriv)->type;
}

/* Documented in header. */
void pbBindDeriv(void) {
    gfnBindCore(GFN_dataOf, PB_Deriv, Deriv_dataOf);
    gfnBindCore(GFN_eq,     PB_Deriv, Deriv_eq);
    gfnBindCore(GFN_gcMark, PB_Deriv, Deriv_gcMark);
    gfnBindCore(GFN_order,  PB_Deriv, Deriv_order);
    gfnBindCore(GFN_sizeOf, PB_Deriv, Deriv_sizeOf);
    gfnBindCore(GFN_typeOf, PB_Deriv, Deriv_typeOf);
}

/* Documented in header. */
static PbType INFO_Deriv = {
    .name = "Deriv"
};
ztype PB_Deriv = &INFO_Deriv;
