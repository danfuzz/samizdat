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
 * Combines up to two element arrays and an additional element into a
 * single new listlet. This can also be used for a single array by
 * passing `size2` as `0`. If `insert` is non-`NULL`, that element is
 * placed in between the two lists of array contents in the result
 */
static zvalue listletFrom(zint size1, const zvalue *elems1, zvalue insert,
                          zint size2, const zvalue *elems2) {
    zint insertCount = (insert == NULL) ? 0 : 1;
    zvalue result = allocListlet(size1 + size2 + insertCount);
    zvalue *resultElems = listletElems(result);

    if (size1 != 0) {
        memcpy(resultElems, elems1, size1 * sizeof(zvalue));
    }

    if (insert != NULL) {
        resultElems[size1] = insert;
    }

    if (size2 != 0) {
        memcpy(resultElems + size1 + insertCount, elems2,
               size2 * sizeof(zvalue));
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
zvalue datListletNth(zvalue listlet, zint n) {
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
    return datListletInsNth(listlet, datSize(listlet), value);
}

/* Documented in header. */
zvalue datListletPrepend(zvalue value, zvalue listlet) {
    return datListletInsNth(listlet, 0, value);
}

/* Documented in header. */
zvalue datListletPutNth(zvalue listlet, zint n, zvalue value) {
    datAssertListlet(listlet);
    datAssertValid(value);

    zint size = datSize(listlet);

    if (n == size) {
        return datListletInsNth(listlet, n, value);
    }

    datAssertNth(listlet, n);

    zvalue result = listletFrom(size, listletElems(listlet), NULL, 0, NULL);

    listletElems(result)[n] = value;
    return result;
}

/* Documented in header. */
zvalue datListletInsNth(zvalue listlet, zint n, zvalue value) {
    datAssertListlet(listlet);
    datAssertValid(value);

    zint size = datSize(listlet);

    if (n != size) {
        datAssertNth(listlet, n);
    }

    zvalue *elems = listletElems(listlet);
    return listletFrom(n, elems, value, size - n, elems + n);
}


/* Documented in header. */
zvalue datListletAdd(zvalue listlet1, zvalue listlet2) {
    datAssertListlet(listlet1);
    datAssertListlet(listlet2);

    return listletFrom(datSize(listlet1), listletElems(listlet1), NULL,
                       datSize(listlet2), listletElems(listlet2));
}

zvalue datListletDelNth(zvalue listlet, zint n) {
    datAssertListlet(listlet);
    datAssertNth(listlet, n);

    zvalue *elems = listletElems(listlet);
    zint size = datSize(listlet);

    return listletFrom(n, elems, NULL, size - n - 1, elems + n + 1);
}

/* Documented in header. */
zvalue datListletFromArray(zint size, const zvalue *values) {
    for (zint i = 0; i < size; i++) {
        datAssertValid(values[i]);
    }

    return listletFrom(size, values, NULL, 0, NULL);
}

/* Documented in header. */
void datArrayFromListlet(zvalue *result, zvalue listlet) {
    datAssertListlet(listlet);
    memcpy(result, listletElems(listlet), listlet->size * sizeof(zvalue));
}
