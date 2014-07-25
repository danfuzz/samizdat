// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include <stdarg.h>

#include "type/Class.h"
#include "type/Data.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/OneOff.h"
#include "type/String.h"
#include "type/Value.h"

#include "impl.h"


//
// Private Definitions
//

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
    return datPayload(list);
}

/**
 * Allocates a list of the given size.
 */
static zvalue allocList(zint size) {
    zvalue result =
        datAllocValue(CLS_List, sizeof(ListInfo) + size * sizeof(zvalue));

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
        utilCpy(zvalue, resultElems, elems1, size1);
    }

    if (insert != NULL) {
        resultElems[size1] = insert;
    }

    if (size2 != 0) {
        utilCpy(zvalue, resultElems + size1 + insertCount, elems2, size2);
    }

    if (DAT_CONSTRUCTION_PARANOIA) {
        for (zint i = 0; i < size; i++) {
            assertValid(resultElems[i]);
        }
    }

    return result;
}


//
// Exported Definitions
//

// Documented in header.
void arrayFromList(zvalue *result, zvalue list) {
    assertHasClass(list, CLS_List);

    ListInfo *info = getInfo(list);

    utilCpy(zvalue, result, info->elems, info->size);
}

// Documented in header.
zvalue listFromArgs(zvalue first, ...) {
    if (first == NULL) {
        return EMPTY_LIST;
    }

    zint size = 1;
    va_list rest;

    va_start(rest, first);
    for (;;) {
        if (va_arg(rest, zvalue) == NULL) {
            break;
        }
        size++;
    }
    va_end(rest);

    zvalue values[size];
    values[0] = first;

    va_start(rest, first);
    for (zint i = 1; i < size; i++) {
        values[i] = va_arg(rest, zvalue);
    }
    va_end(rest);

    return listFromArray(size, values);
}

// Documented in header.
zvalue listFromArray(zint size, const zvalue *values) {
    for (zint i = 0; i < size; i++) {
        assertValid(values[i]);
    }

    return listFrom(size, values, NULL, 0, NULL);
}


//
// Class Definition
//

// Documented in header.
zvalue EMPTY_LIST = NULL;

// Documented in header.
METH_IMPL(List, cat) {
    if (argCount == 1) {
        return args[0];
    }

    zint size = 0;

    for (zint i = 0; i < argCount; i++) {
        size += getInfo(args[i])->size;
    }

    zvalue elems[size];

    for (zint i = 0, at = 0; i < argCount; i++) {
        arrayFromList(&elems[at], args[i]);
        at += getInfo(args[i])->size;
    }

    return listFrom(size, elems, NULL, 0, NULL);
}

// Documented in header.
METH_IMPL(List, collect) {
    zvalue list = args[0];
    zvalue function = (argCount > 1) ? args[1] : NULL;

    if (function == NULL) {
        // Collecting a list (without filtering) results in that same list.
        return list;
    }

    ListInfo *info = getInfo(list);
    zvalue *elems = info->elems;
    zint size = info->size;
    zvalue result[size];
    zint at = 0;

    for (zint i = 0; i < size; i++) {
        zvalue one = FUN_CALL(function, elems[i]);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return listFromArray(at, result);
}

// Documented in header.
METH_IMPL(List, del) {
    zvalue list = args[0];
    zvalue n = args[1];

    ListInfo *info = getInfo(list);
    zvalue *elems = info->elems;
    zint size = info->size;
    zint index = seqNthIndexLenient(n);

    if ((index < 0) || (index >= size)) {
        return list;
    }

    return listFrom(index, elems, NULL, size - index - 1, &elems[index + 1]);
}

// Documented in header.
METH_IMPL(List, fetch) {
    zvalue list = args[0];
    ListInfo *info = getInfo(list);

    switch (info->size) {
        case 0: {
            return NULL;
        }
        case 1: {
            return info->elems[0];
        }
        default: {
            die("Invalid to call `fetch` on list with size > 1.");
        }
    }
}

// Documented in header.
METH_IMPL(List, gcMark) {
    zvalue list = args[0];
    ListInfo *info = getInfo(list);
    zvalue *elems = info->elems;
    zint size = info->size;

    for (zint i = 0; i < size; i++) {
        datMark(elems[i]);
    }

    return NULL;
}

// Documented in header.
METH_IMPL(List, get_size) {
    zvalue list = args[0];
    return intFromZint(getInfo(list)->size);
}

// Documented in header.
METH_IMPL(List, nth) {
    zvalue list = args[0];
    zvalue n = args[1];

    ListInfo *info = getInfo(list);
    zint index = seqNthIndexStrict(info->size, n);

    return (index < 0) ? NULL : datFrameAdd(info->elems[index]);
}

// Documented in header.
METH_IMPL(List, put) {
    zvalue list = args[0];
    zvalue n = args[1];
    zvalue value = args[2];

    ListInfo *info = getInfo(list);
    zvalue *elems = info->elems;
    zint size = info->size;
    zint index = seqPutIndexStrict(size, n);

    if (index == size) {
        // This is an append operation.
        return listFrom(size, elems, value, 0, NULL);
    }

    return listFrom(index, elems, value, size - index - 1, &elems[index + 1]);
}

// Documented in header.
METH_IMPL(List, reverse) {
    zvalue list = args[0];

    ListInfo *info = getInfo(list);
    zint size = info->size;
    zvalue *elems = info->elems;
    zvalue arr[size];

    for (zint i = 0, j = size - 1; i < size; i++, j--) {
        arr[i] = elems[j];
    }

    return listFrom(size, arr, NULL, 0, NULL);
}

// Documented in header.
static zvalue doSlice(bool inclusive, zint argCount, const zvalue *args) {
    zvalue list = args[0];
    ListInfo *info = getInfo(list);
    zint start;
    zint end;

    seqConvertSliceArgs(&start, &end, inclusive, info->size, argCount, args);

    if (start == -1) {
        return NULL;
    } else {
        return listFrom(end - start, &info->elems[start], NULL, 0, NULL);
    }
}

// Documented in header.
METH_IMPL(List, sliceExclusive) {
    return doSlice(false, argCount, args);
}

// Documented in header.
METH_IMPL(List, sliceInclusive) {
    return doSlice(true, argCount, args);
}

// Documented in header.
METH_IMPL(List, totalEq) {
    zvalue value = args[0];
    zvalue other = args[1];
    ListInfo *info1 = getInfo(value);
    ListInfo *info2 = getInfo(other);
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

    return value;
}

// Documented in header.
METH_IMPL(List, totalOrder) {
    zvalue value = args[0];
    zvalue other = args[1];
    ListInfo *info1 = getInfo(value);
    ListInfo *info2 = getInfo(other);
    zvalue *e1 = info1->elems;
    zvalue *e2 = info2->elems;
    zint size1 = info1->size;
    zint size2 = info2->size;
    zint size = (size1 < size2) ? size1 : size2;

    for (zint i = 0; i < size; i++) {
        zorder result = valZorder(e1[i], e2[i]);
        if (result != ZSAME) {
            return intFromZint(result);
        }
    }

    if (size1 == size2) {
        return INT_0;
    }

    return (size1 < size2) ? INT_NEG1 : INT_1;
}

// Documented in header.
METH_IMPL(List, valueList) {
    zvalue list = args[0];

    return list;
}

/** Initializes the module. */
MOD_INIT(List) {
    MOD_USE(Sequence);
    MOD_USE(OneOff);

    CLS_List = makeCoreClass(stringFromUtf8(-1, "List"), CLS_Data);

    METH_BIND(List, cat);
    METH_BIND(List, collect);
    METH_BIND(List, del);
    METH_BIND(List, fetch);
    METH_BIND(List, gcMark);
    METH_BIND(List, get_size);
    METH_BIND(List, nth);
    METH_BIND(List, put);
    METH_BIND(List, reverse);
    METH_BIND(List, sliceExclusive);
    METH_BIND(List, sliceInclusive);
    METH_BIND(List, totalEq);
    METH_BIND(List, totalOrder);
    METH_BIND(List, valueList);
    seqBind(CLS_List);

    EMPTY_LIST = allocList(0);
    datImmortalize(EMPTY_LIST);
}

// Documented in header.
zvalue CLS_List = NULL;
