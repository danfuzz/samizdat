/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

#include <stdlib.h>
#include <string.h>


/*
 * Helper definitions / functions
 */

/** The empty listlet, lazily initialized. */
static zvalue theEmptyListlet = NULL;

/**
 * Allocates a listlet of the given size.
 */
static zvalue allocListlet(zint size) {
    return datAllocValue(DAT_LISTLET, size, size * sizeof(zvalue));
}

/**
 * Gets the array of `zvalue` elements from a listlet.
 */
static zvalue *listletElems(zvalue listlet) {
    return ((DatListlet *) listlet)->elems;
}

/**
 * Combines two element arrays into a single new listlet. This
 * can also be used for a single array by passing `size2` as `0`.
 */
static zvalue listletFromTwoArrays(zint size1, const zvalue *elems1,
                                   zint size2, const zvalue *elems2) {
    zvalue result = allocListlet(size1 + size2);
    zvalue *resultElems = listletElems(result);

    memcpy(resultElems, elems1, size1 * sizeof(zvalue));

    if (size2 != 0) {
        memcpy(resultElems + size1, elems2, size2 * sizeof(zvalue));
    }

    return result;
}


/*
 * Module functions
 */

/* Documented in header. */
zorder datListletOrder(zvalue v1, zvalue v2) {
    zvalue *e1 = listletElems(v1);
    zvalue *e2 = listletElems(v2);
    zint sz1 = datSize(v1);
    zint sz2 = datSize(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zorder result = datOrder(e1[i], e2[i]);
        if (result != ZSAME) {
            return result;
        }
    }

    if (sz1 == sz2) {
        return ZSAME;
    }

    return (sz1 < sz2) ? ZLESS : ZMORE;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datListletGet(zvalue listlet, zint n) {
    datAssertListlet(listlet);
    datAssertNth(listlet, n);
    return listletElems(listlet)[n];
}

/* Documented in header. */
zvalue datListletEmpty(void) {
    if (theEmptyListlet == NULL) {
        theEmptyListlet = allocListlet(0);
    }

    return theEmptyListlet;
}

/* Documented in header. */
zvalue datListletAppend(zvalue listlet, zvalue value) {
    datAssertListlet(listlet);
    datAssertValid(value);

    return listletFromTwoArrays(datSize(listlet), listletElems(listlet),
                                1, &value);
}

/* Documented in header. */
zvalue datListletPrepend(zvalue value, zvalue listlet) {
    datAssertValid(value);
    datAssertListlet(listlet);

    return listletFromTwoArrays(1, &value,
                                datSize(listlet), listletElems(listlet));
}

/* Documented in header. */
zvalue datListletAdd(zvalue listlet1, zvalue listlet2) {
    datAssertListlet(listlet1);
    datAssertListlet(listlet2);

    return listletFromTwoArrays(datSize(listlet1), listletElems(listlet1),
                                datSize(listlet2), listletElems(listlet2));
}

zvalue datListletDelete(zvalue listlet, zint n) {
    datAssertListlet(listlet);
    datAssertNth(listlet, n);

    zvalue *elems = listletElems(listlet);
    zint size = datSize(listlet);

    return listletFromTwoArrays(n, elems, size - n - 1, elems + n + 1);
}

/* Documented in header. */
zvalue datListletFromArray(zint size, const zvalue *values) {
    for (zint i = 0; i < size; i++) {
        datAssertValid(values[i]);
    }

    return listletFromTwoArrays(size, values, 0, NULL);
}

/* Documented in header. */
void datArrayFromListlet(zvalue listlet, zvalue *values) {
    datAssertListlet(listlet);
    memcpy(values, listletElems(listlet), listlet->size * sizeof(zvalue));
}
