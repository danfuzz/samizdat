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
} DatDeriv;

/**
 * Gets a pointer to the value's info.
 */
static DatDeriv *derivInfo(zvalue deriv) {
    return datPayload(deriv);
}

/**
 * Allocates and initializes a derived value, without doing error-checking
 * on the arguments.
 */
static zvalue newDeriv(zvalue type, zvalue data) {
    zvalue result = datAllocValue(DAT_Deriv, sizeof(DatDeriv));
    DatDeriv *info = derivInfo(result);

    info->type = type;
    info->data = data;
    return result;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datDerivFrom(zvalue type, zvalue data) {
    datAssertValid(type);

    if (data != NULL) {
        datAssertValid(data);
    }

    return newDeriv(type, data);
}

/* Documented in header. */
zvalue datDerivType(zvalue deriv) {
    datAssertDeriv(deriv);
    return derivInfo(deriv)->type;
}

/* Documented in header. */
zvalue datDerivData(zvalue deriv) {
    datAssertDeriv(deriv);
    return derivInfo(deriv)->data;
}


/*
 * Type binding
 */

/* Documented in header. */
static zint derivSizeOf(zvalue deriv) {
    return (derivInfo(deriv)->data == NULL) ? 0 : 1;
}

/* Documented in header. */
static void derivGcMark(zvalue deriv) {
    DatDeriv *info = derivInfo(deriv);

    datMark(info->type);
    datMark(info->data);
}

/* Documented in header. */
static bool derivEq(zvalue v1, zvalue v2) {
    DatDeriv *info1 = derivInfo(v1);
    DatDeriv *info2 = derivInfo(v2);

    if (info1->data == NULL) {
        if (info2->data != NULL) {
            return false;
        }
    } else if (info2->data == NULL) {
        return false;
    } else if (!datEq(info1->data, info2->data)) {
        return false;
    }

    return datEq(info1->type, info2->type);
}

/* Documented in header. */
static zorder derivOrder(zvalue v1, zvalue v2) {
    DatDeriv *info1 = derivInfo(v1);
    DatDeriv *info2 = derivInfo(v2);

    zorder result = datOrder(info1->type, info2->type);

    if (result != ZSAME) {
        return result;
    } else if (info1->data == NULL) {
        return (info2->data == NULL) ? ZSAME : ZLESS;
    } else if (info2->data == NULL) {
        return ZMORE;
    } else {
        return datOrder(info1->data, info2->data);
    }
}

/* Documented in header. */
static DatType INFO_Deriv = {
    .id = DAT_DERIV,
    .name = "Deriv",
    .sizeOf = derivSizeOf,
    .typeOf = datDerivType,
    .gcMark = derivGcMark,
    .gcFree = NULL,
    .eq = derivEq,
    .order = derivOrder
};
ztype DAT_Deriv = &INFO_Deriv;
