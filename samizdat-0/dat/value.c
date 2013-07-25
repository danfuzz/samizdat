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
static ValueInfo *valueInfo(zvalue value) {
    return &((DatValue *) value)->info;
}

/**
 * Allocates and initializes a derived value, without doing error-checking
 * on the arguments.
 */
static zvalue newValue(zvalue type, zvalue data) {
    zvalue result = datAllocValue(DAT_VALUE, 0, sizeof(ValueInfo));
    ValueInfo *info = valueInfo(result);

    result->size = (data == NULL) ? 0 : 1;
    info->type = type;
    info->data = data;
    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
bool datValueEq(zvalue v1, zvalue v2) {
    ValueInfo *info1 = valueInfo(v1);
    ValueInfo *info2 = valueInfo(v2);

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
zorder datValueOrder(zvalue v1, zvalue v2) {
    ValueInfo *info1 = valueInfo(v1);
    ValueInfo *info2 = valueInfo(v2);

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
void datValueMark(zvalue value) {
    ValueInfo *info = valueInfo(value);

    datMark(info->type);
    if (info->data != NULL) {
        datMark(info->data);
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datValueFrom(zvalue type, zvalue data) {
    datAssertValid(type);

    if (data != NULL) {
        datAssertValid(data);
    }

    return newValue(type, data);
}

/* Documented in header. */
zvalue datValueType(zvalue value) {
    datAssertDeriv(value);
    return valueInfo(value)->type;
}

/* Documented in header. */
bool datValueTypeIs(zvalue value, zvalue type) {
    return datEq(datValueType(value), type);
}

/* Documented in header. */
zvalue datValueData(zvalue value) {
    datAssertDeriv(value);
    return valueInfo(value)->data;
}
