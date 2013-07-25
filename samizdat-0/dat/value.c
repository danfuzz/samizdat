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
 * Gets a pointer to the value's info.
 */
static DerivInfo *derivInfo(zvalue value) {
    return &((DatDeriv *) value)->info;
}

/**
 * Allocates and initializes a derived value, without doing error-checking
 * on the arguments.
 */
static zvalue newDeriv(zvalue type, zvalue data) {
    zvalue result = datAllocValue(DAT_DERIV, 0, sizeof(DerivInfo));
    DerivInfo *info = derivInfo(result);

    result->size = (data == NULL) ? 0 : 1;
    info->type = type;
    info->data = data;
    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
bool datDerivEq(zvalue v1, zvalue v2) {
    DerivInfo *info1 = derivInfo(v1);
    DerivInfo *info2 = derivInfo(v2);

    if (!datEq(info1->type, info2->type)) {
        return false;
    }

    if (info1->data == NULL) {
        return (info2->data == NULL);
    } else if (info2->data == NULL) {
        return false;
    } else {
        return datEq(info1->data, info2->data);
    }
}

/* Documented in header. */
zorder datDerivOrder(zvalue v1, zvalue v2) {
    DerivInfo *info1 = derivInfo(v1);
    DerivInfo *info2 = derivInfo(v2);

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
void datDerivMark(zvalue value) {
    DerivInfo *info = derivInfo(value);

    datMark(info->type);
    if (info->data != NULL) {
        datMark(info->data);
    }
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
bool datDerivTypeIs(zvalue deriv, zvalue type) {
    return datEq(datDerivType(deriv), type);
}

/* Documented in header. */
zvalue datDerivData(zvalue deriv) {
    datAssertDeriv(deriv);
    return derivInfo(deriv)->data;
}
