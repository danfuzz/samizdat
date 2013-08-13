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
 * List structure.
 */
typedef struct {
    /** Number of elements. */
    zint size;

    /** List elements, in index order. */
    zvalue elems[/*size*/];
} ListInfo;

/**
 * Gets the array of `zvalue` elements from a list.
 */
static zvalue *listElems(zvalue list) {
    return ((ListInfo *) pbPayload(list))->elems;
}

/**
 * Gets the size of a list.
 */
static zint listSizeOf(zvalue list) {
    return ((ListInfo *) pbPayload(list))->size;
}

/**
 * Allocates a list of the given size.
 */
static zvalue allocList(zint size) {
    zvalue result =
        pbAllocValue(TYPE_List, sizeof(ListInfo) + size * sizeof(zvalue));

    ((ListInfo *) pbPayload(result))->size = size;
    return result;
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
    zint size = size1 + size2 + insertCount;

    if (size == 0) {
        return EMPTY_LIST;
    }

    zvalue result = allocList(size);
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
 * Exported functions
 */

/* Documented in header. */
void datAssertList(zvalue value) {
    assertHasType(value, TYPE_List);
}

/* Documented in header. */
void arrayFromList(zvalue *result, zvalue list) {
    datAssertList(list);
    memcpy(result, listElems(list), listSizeOf(list) * sizeof(zvalue));
}

/* Documented in header. */
zvalue listAdd(zvalue list1, zvalue list2) {
    datAssertList(list1);
    datAssertList(list2);

    zint size1 = listSizeOf(list1);
    zint size2 = listSizeOf(list2);

    if (size1 == 0) {
        return list2;
    } else if (size2 == 0) {
        return list1;
    }

    return listFrom(size1, listElems(list1), NULL, size2, listElems(list2));
}

/* Documented in header. */
zvalue listAppend(zvalue list, zvalue value) {
    datAssertList(list);
    return listInsNth(list, listSizeOf(list), value);
}

/* Documented in header. */
zvalue listDelNth(zvalue list, zint n) {
    datAssertList(list);

    zvalue *elems = listElems(list);
    zint size = listSizeOf(list);

    pbAssertNth(size, n);

    return listFrom(n, elems, NULL, size - n - 1, elems + n + 1);
}

/* Documented in header. */
zvalue listFromArray(zint size, const zvalue *values) {
    for (zint i = 0; i < size; i++) {
        pbAssertValid(values[i]);
    }

    return listFrom(size, values, NULL, 0, NULL);
}

/* Documented in header. */
zvalue listInsNth(zvalue list, zint n, zvalue value) {
    datAssertList(list);
    pbAssertValid(value);

    zint size = listSizeOf(list);
    zvalue *elems = listElems(list);

    pbAssertNthOrSize(size, n);

    return listFrom(n, elems, value, size - n, elems + n);
}

/* Documented in header. */
zvalue listNth(zvalue list, zint n) {
    datAssertList(list);

    if ((n < 0) || (n >= listSizeOf(list))) {
        return NULL;
    }

    return listElems(list)[n];
}

/* Documented in header. */
zvalue listPutNth(zvalue list, zint n, zvalue value) {
    datAssertList(list);
    pbAssertValid(value);

    zint size = listSizeOf(list);

    pbAssertNthOrSize(size, n);

    if (n == size) {
        return listInsNth(list, n, value);
    }

    zvalue result = listFrom(size, listElems(list), NULL, 0, NULL);

    listElems(result)[n] = value;
    return result;
}

/* Documented in header. */
zvalue listSlice(zvalue list, zint start, zint end) {
    datAssertList(list);
    pbAssertSliceRange(listSizeOf(list), start, end);

    return listFrom(end - start, &listElems(list)[start], NULL, 0, NULL);
}


/*
 * Type binding
 */

/* Documented in header. */
zvalue EMPTY_LIST = NULL;

/* Documented in header. */
METH_IMPL(List, eq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zint sz1 = listSizeOf(v1);
    zint sz2 = listSizeOf(v2);

    if (sz1 != sz2) {
        return NULL;
    }

    zvalue *e1 = listElems(v1);
    zvalue *e2 = listElems(v2);

    for (zint i = 0; i < sz1; i++) {
        if (!pbEq(e1[i], e2[i])) {
            return NULL;
        }
    }

    return v2;
}

/* Documented in header. */
METH_IMPL(List, gcMark) {
    zvalue list = args[0];
    zint size = listSizeOf(list);
    zvalue *elems = listElems(list);

    for (zint i = 0; i < size; i++) {
        pbMark(elems[i]);
    }

    return NULL;
}

/* Documented in header. */
METH_IMPL(List, order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    zvalue *e1 = listElems(v1);
    zvalue *e2 = listElems(v2);
    zint sz1 = listSizeOf(v1);
    zint sz2 = listSizeOf(v2);
    zint sz = (sz1 < sz2) ? sz1 : sz2;

    for (zint i = 0; i < sz; i++) {
        zorder result = pbOrder(e1[i], e2[i]);
        if (result != ZSAME) {
            return intFromZint(result);
        }
    }

    if (sz1 == sz2) {
        return PB_0;
    }

    return (sz1 < sz2) ? PB_NEG1 : PB_1;
}

/* Documented in header. */
METH_IMPL(List, size) {
    zvalue list = args[0];
    return intFromZint(listSizeOf(list));
}

/* Documented in header. */
void datBindList(void) {
    TYPE_List = coreTypeFromName(stringFromUtf8(-1, "List"));
    gfnBindCore(GFN_eq,     TYPE_List, List_eq);
    gfnBindCore(GFN_gcMark, TYPE_List, List_gcMark);
    gfnBindCore(GFN_order,  TYPE_List, List_order);
    gfnBindCore(GFN_size,   TYPE_List, List_size);

    EMPTY_LIST = allocList(0);
    pbImmortalize(EMPTY_LIST);
}

/* Documented in header. */
zvalue TYPE_List = NULL;
