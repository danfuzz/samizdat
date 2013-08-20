/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

#include "impl.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"

#include <string.h>


/*
 * Private Definitions
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
 * Gets a pointer to the value's info.
 */
static ListInfo *getInfo(zvalue list) {
    return pbPayload(list);
}

/**
 * Allocates a list of the given size.
 */
static zvalue allocList(zint size) {
    zvalue result =
        pbAllocValue(TYPE_List, sizeof(ListInfo) + size * sizeof(zvalue));

    getInfo(result)->size = size;
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
    zvalue *resultElems = getInfo(result)->elems;

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
 * Exported Definitions
 */

/* Documented in header. */
void assertList(zvalue value) {
    assertHasType(value, TYPE_List);
}

/* Documented in header. */
void arrayFromList(zvalue *result, zvalue list) {
    assertList(list);

    ListInfo *info = getInfo(list);

    memcpy(result, info->elems, info->size * sizeof(zvalue));
}

/* Documented in header. */
zvalue listCat(zvalue list1, zvalue list2) {
    assertList(list1);
    assertList(list2);

    ListInfo *info1 = getInfo(list1);
    ListInfo *info2 = getInfo(list2);
    zint size1 = info1->size;
    zint size2 = info2->size;

    if (size1 == 0) {
        return list2;
    } else if (size2 == 0) {
        return list1;
    }

    return listFrom(size1, info1->elems, NULL, size2, info2->elems);
}

/* Documented in header. */
zvalue listDelNth(zvalue list, zint n) {
    assertList(list);

    ListInfo *info = getInfo(list);
    zvalue *elems = info->elems;
    zint size = info->size;

    assertNth(size, n);

    return listFrom(n, elems, NULL, size - n - 1, elems + n + 1);
}

/* Documented in header. */
zvalue listFromArray(zint size, const zvalue *values) {
    for (zint i = 0; i < size; i++) {
        assertValid(values[i]);
    }

    return listFrom(size, values, NULL, 0, NULL);
}

/* Documented in header. */
zvalue listInsNth(zvalue list, zint n, zvalue value) {
    assertList(list);
    assertValid(value);

    ListInfo *info = getInfo(list);
    zvalue *elems = info->elems;
    zint size = info->size;

    assertNthOrSize(size, n);

    return listFrom(n, elems, value, size - n, elems + n);
}

/* Documented in header. */
zvalue listNth(zvalue list, zint n) {
    assertList(list);

    ListInfo *info = getInfo(list);

    if ((n < 0) || (n >= info->size)) {
        return NULL;
    }

    return info->elems[n];
}

/* Documented in header. */
zvalue listPutNth(zvalue list, zint n, zvalue value) {
    assertList(list);
    assertValid(value);

    ListInfo *info = getInfo(list);
    zint size = info->size;

    assertNthOrSize(size, n);

    if (n == size) {
        return listInsNth(list, n, value);
    }

    zvalue result = listFrom(size, info->elems, NULL, 0, NULL);

    getInfo(result)->elems[n] = value;
    return result;
}

/* Documented in header. */
zvalue listSlice(zvalue list, zint start, zint end) {
    assertList(list);

    ListInfo *info = getInfo(list);

    assertSliceRange(info->size, start, end);
    return listFrom(end - start, &info->elems[start], NULL, 0, NULL);
}


/*
 * Type Definition
 */

/* Documented in header. */
zvalue EMPTY_LIST = NULL;

/* Documented in header. */
METH_IMPL(List, eq) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    ListInfo *info1 = getInfo(v1);
    ListInfo *info2 = getInfo(v2);
    zint size1 = info1->size;
    zint size2 = info2->size;

    if (size1 != size2) {
        return NULL;
    }

    zvalue *e1 = info1->elems;
    zvalue *e2 = info2->elems;

    for (zint i = 0; i < size1; i++) {
        if (!valEq(e1[i], e2[i])) {
            return NULL;
        }
    }

    return v2;
}

/* Documented in header. */
METH_IMPL(List, gcMark) {
    zvalue list = args[0];
    ListInfo *info = getInfo(list);
    zvalue *elems = info->elems;
    zint size = info->size;

    for (zint i = 0; i < size; i++) {
        pbMark(elems[i]);
    }

    return NULL;
}

/* Documented in header. */
METH_IMPL(List, nth) {
    zvalue list = args[0];
    zvalue n = args[1];

    ListInfo *info = getInfo(list);
    zint index = collNthIndexStrict(info->size, n);

    return listNth(list, index);
}

/* Documented in header. */
METH_IMPL(List, order) {
    zvalue v1 = args[0];
    zvalue v2 = args[1];
    ListInfo *info1 = getInfo(v1);
    ListInfo *info2 = getInfo(v2);
    zvalue *e1 = info1->elems;
    zvalue *e2 = info2->elems;
    zint size1 = info1->size;
    zint size2 = info2->size;
    zint size = (size1 < size2) ? size1 : size2;

    for (zint i = 0; i < size; i++) {
        zorder result = valOrder(e1[i], e2[i]);
        if (result != ZSAME) {
            return intFromZint(result);
        }
    }

    if (size1 == size2) {
        return PB_0;
    }

    return (size1 < size2) ? PB_NEG1 : PB_1;
}

/* Documented in header. */
METH_IMPL(List, size) {
    zvalue list = args[0];
    return intFromZint(getInfo(list)->size);
}

/* Documented in header. */
void datBindList(void) {
    TYPE_List = coreTypeFromName(stringFromUtf8(-1, "List"), false);
    METH_BIND(List, eq);
    METH_BIND(List, gcMark);
    METH_BIND(List, nth);
    METH_BIND(List, order);
    METH_BIND(List, size);
    seqBind(TYPE_List);

    EMPTY_LIST = allocList(0);
    pbImmortalize(EMPTY_LIST);
}

/* Documented in header. */
zvalue TYPE_List = NULL;
