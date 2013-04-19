/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

#include <stdlib.h>


/*
 * Helper definitions
 */

/**
 * Gets a pointer to the highlet's info.
 */
static HighletInfo *highletInfo(zvalue highlet) {
    return &((DatHighlet *) highlet)->info;
}

/**
 * Allocates and initializes a highlet, without doing error-checking
 * on the arguments.
 */
static zvalue newHighlet(zvalue type, zvalue value) {
    zvalue result = datAllocValue(DAT_HIGHLET, 0, sizeof(HighletInfo));
    HighletInfo *info = highletInfo(result);

    result->size = (value == NULL) ? 0 : 1;
    info->type = type;
    info->value = value;
    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
zorder datHighletOrder(zvalue v1, zvalue v2) {
    HighletInfo *info1 = highletInfo(v1);
    HighletInfo *info2 = highletInfo(v2);

    zorder result = datOrder(info1->type, info2->type);

    if (result != ZSAME) {
        return result;
    } else if (info1->value == NULL) {
        return (info2->value == NULL) ? ZSAME : ZLESS;
    } else if (info2->value == NULL) {
        return ZMORE;
    } else {
        return datOrder(info1->value, info2->value);
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datHighletType(zvalue highlet) {
    datAssertHighlet(highlet);
    return highletInfo(highlet)->type;
}

/* Documented in header. */
zvalue datHighletValue(zvalue highlet) {
    datAssertHighlet(highlet);
    return highletInfo(highlet)->value;
}

/* Documented in header. */
zvalue datHighletFromType(zvalue type) {
    datAssertValid(type);
    return newHighlet(type, NULL);
}

/* Documented in header. */
zvalue datHighletFrom(zvalue type, zvalue value) {
    datAssertValid(type);

    if (value != NULL) {
        datAssertValid(value);
    }

    return newHighlet(type, value);
}

/* Documented in header. */
bool datHighletHasType(zvalue highlet, zvalue type) {
    return (datOrder(datHighletType(highlet), type) == 0);
}

/* Documented in header. */
void datHighletAssertType(zvalue highlet, zvalue type) {
    if (!datHighletHasType(highlet, type)) {
        die("Type mismatch.");
    }
}
