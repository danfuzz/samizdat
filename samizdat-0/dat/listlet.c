/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. See the associated file "LICENSE.md" for details.
 */

#include "impl.h"

#include <stdlib.h>
#include <string.h>


/*
 * Helper definitions
 */

/** The empty listlet, lazily initialized. */
static zvalue theEmptyListlet = NULL;


/*
 * Module functions
 */

/* Documented in header. */
zvalue datAllocListlet(zint size) {
    return datAllocValue(DAT_LISTLET, size, size * sizeof(zvalue));
}

/* Documented in header. */
zvalue *datListletElems(zvalue listlet) {
    datAssertListlet(listlet);

    return ((DatListlet *) listlet)->elems;
}

/* Documented in header. */
zcomparison datListletCompare(zvalue v1, zvalue v2) {
    zvalue *e1 = datListletElems(v1);
    zvalue *e2 = datListletElems(v2);
    zint sz1 = datSize(v1);
    zint sz2 = datSize(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zcomparison result = datCompare(e1[i], e2[i]);
        if (result != ZEQUAL) {
            return result;
        }
    }

    if (sz1 == sz2) {
        return ZEQUAL;
    }

    return (sz1 < sz2) ? ZLESS : ZMORE;
}


/*
 * Exported functions
 */

/* Documented in header. */
zvalue datListletGet(zvalue listlet, zint n) {
    datAssertNth(listlet, n);
    return datListletElems(listlet)[n];
}

/* Documented in header. */
zint datListletGetInt(zvalue listlet, zint n) {
    zvalue v = datListletGet(listlet, n);
    return datIntletToInt(v);
}

/* Documented in header. */
zvalue datListletEmpty(void) {
    if (theEmptyListlet == NULL) {
        theEmptyListlet = datAllocListlet(0);
    }

    return theEmptyListlet;
}

/* Documented in header. */
zvalue datListletFromValues(zvalue *values, zint size) {
    zvalue result = datAllocListlet(size);
    zvalue *resultElems = datListletElems(result);

    for (zint i = 0; i < size; i++) {
        datAssertValid(values[i]);
        resultElems[i] = values[i];
    }

    return result;
}

/* Documented in header. */
zvalue datListletAppend(zvalue listlet, zvalue value) {
    datAssertListlet(listlet);

    zint oldSize = listlet->size;
    zint size = oldSize + 1;
    zvalue result = datAllocListlet(size);
    zvalue *resultElems = datListletElems(result);

    memcpy(resultElems, datListletElems(listlet), oldSize * sizeof(zvalue));
    resultElems[oldSize] = value;

    return result;
}

zvalue datListletDelete(zvalue listlet, zint n) {
    datAssertListlet(listlet);
    datAssertNth(listlet, n);

    zint oldSize = listlet->size;
    zint size = oldSize - 1;
    zvalue result = datAllocListlet(size);
    zvalue *resultElems = datListletElems(result);
    zvalue *oldElems = datListletElems(listlet);

    memcpy(resultElems, oldElems, n * sizeof(zvalue));
    memcpy(resultElems + n, oldElems + n + 1, (size - n) * sizeof(zvalue));

    return result;
}
