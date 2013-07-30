/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"

#include <string.h>


/*
 * Helper definitions / functions
 */

/**
 * Allocates a list of the given size.
 */
static zvalue allocList(zint size) {
    return datAllocValue(DAT_List, size, size * sizeof(zvalue));
}

/**
 * Gets the array of `zvalue` elements from a list.
 */
static zvalue *listElems(zvalue list) {
    return ((DatList *) list)->elems;
}

/**
 * Combines up to two element arrays and an additional element into a
 * single new list. This can also be used for a single array by
 * passing `size2` as `0`. If `insert` is non-`NULL`, that element is
 * placed in between the two lists of array contents in the result
 */
static zvalue listFrom(zint size1, const zvalue *elems1, zvalue insert,
                       zint size2, const zvalue *elems2) {
    zint insertCount = (insert == NULL) ? 0 : 1;
    zvalue result = allocList(size1 + size2 + insertCount);
    zvalue *resultElems = listElems(result);

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
bool datListEq(zvalue v1, zvalue v2) {
    zvalue *e1 = listElems(v1);
    zvalue *e2 = listElems(v2);
    zint size = datSize(v1);

    for (zint i = 0; i < size; i++) {
        if (!datEq(e1[i], e2[i])) {
            return false;
        }
    }

    return true;
}

/* Documented in header. */
zorder datListOrder(zvalue v1, zvalue v2) {
    zvalue *e1 = listElems(v1);
    zvalue *e2 = listElems(v2);
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

/* Documented in header. */
void datListMark(zvalue value) {
    zint size = datSize(value);
    zvalue *elems = listElems(value);

    for (zint i = 0; i < size; i++) {
        datMark(elems[i]);
    }
}


/*
 * Exported functions
 */

/* Documented in header. */
void datArrayFromList(zvalue *result, zvalue list) {
    datAssertList(list);
    memcpy(result, listElems(list), list->size * sizeof(zvalue));
}

/* Documented in header. */
zvalue datListAdd(zvalue list1, zvalue list2) {
    datAssertList(list1);
    datAssertList(list2);

    zint size1 = datSize(list1);
    zint size2 = datSize(list2);

    if (size1 == 0) {
        return list2;
    } else if (size2 == 0) {
        return list1;
    }

    return listFrom(size1, listElems(list1), NULL, size2, listElems(list2));
}

/* Documented in header. */
zvalue datListAppend(zvalue list, zvalue value) {
    return datListInsNth(list, datSize(list), value);
}

/* Documented in header. */
zvalue datListDelNth(zvalue list, zint n) {
    datAssertList(list);
    datAssertNth(list, n);

    zvalue *elems = listElems(list);
    zint size = datSize(list);

    return listFrom(n, elems, NULL, size - n - 1, elems + n + 1);
}

/* Documented in header. */
zvalue datListFromArray(zint size, const zvalue *values) {
    for (zint i = 0; i < size; i++) {
        datAssertValid(values[i]);
    }

    return listFrom(size, values, NULL, 0, NULL);
}

/* Documented in header. */
zvalue datListInsNth(zvalue list, zint n, zvalue value) {
    datAssertList(list);
    datAssertValid(value);
    datAssertNthOrSize(list, n);

    zint size = datSize(list);
    zvalue *elems = listElems(list);

    return listFrom(n, elems, value, size - n, elems + n);
}

/* Documented in header. */
zvalue datListNth(zvalue list, zint n) {
    datAssertList(list);
    return datHasNth(list, n) ? listElems(list)[n] : NULL;
}

/* Documented in header. */
zvalue datListPutNth(zvalue list, zint n, zvalue value) {
    datAssertList(list);
    datAssertValid(value);

    zint size = datSize(list);

    if (n == size) {
        return datListInsNth(list, n, value);
    }

    datAssertNth(list, n);

    zvalue result = listFrom(size, listElems(list), NULL, 0, NULL);

    listElems(result)[n] = value;
    return result;
}

/* Documented in header. */
zvalue datListSlice(zvalue list, zint start, zint end) {
    datAssertList(list);
    datAssertSliceRange(list, start, end);

    return listFrom(end - start, &listElems(list)[start], NULL, 0, NULL);
}


/*
 * Type binding
 */

/* Documented in header. */
static zint listSizeOf(zvalue list) {
    return ((DatList *) list)->header.size;
}

/* Documented in header. */
static DatType INFO_List = {
    .id = DAT_LIST,
    .name = "List",
    .sizeOf = listSizeOf
};
ztype DAT_List = &INFO_List;
