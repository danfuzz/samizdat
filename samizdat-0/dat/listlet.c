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

    zint oldSize = listlet->size;
    zint size = oldSize + 1;
    zvalue result = allocListlet(size);
    zvalue *resultElems = listletElems(result);

    memcpy(resultElems, listletElems(listlet), oldSize * sizeof(zvalue));
    resultElems[oldSize] = value;

    return result;
}

zvalue datListletDelete(zvalue listlet, zint n) {
    datAssertListlet(listlet);
    datAssertNth(listlet, n);

    zint oldSize = listlet->size;
    zint size = oldSize - 1;
    zvalue result = allocListlet(size);
    zvalue *resultElems = listletElems(result);
    zvalue *oldElems = listletElems(listlet);

    memcpy(resultElems, oldElems, n * sizeof(zvalue));
    memcpy(resultElems + n, oldElems + n + 1, (size - n) * sizeof(zvalue));

    return result;
}

/* Documented in header. */
zvalue datListletFromValues(zvalue *values, zint size) {
    zvalue result = allocListlet(size);
    zvalue *resultElems = listletElems(result);

    for (zint i = 0; i < size; i++) {
        datAssertValid(values[i]);
        resultElems[i] = values[i];
    }

    return result;
}

/* Documented in header. */
void datListletToValues(zvalue listlet, zvalue *values) {
    datAssertListlet(listlet);
    memcpy(values, listletElems(listlet), listlet->size * sizeof(zvalue));
}
