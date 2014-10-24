// Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Box.h"
#include "type/Core.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/define.h"

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
 * placed in between the two lists of array contents in the result.
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

/**
 * Helper that does most of the work of the `slice*` methods.
 */
static zvalue doSlice(zvalue ths, bool inclusive,
        zvalue startArg, zvalue endArg) {
    ListInfo *info = getInfo(ths);
    zint start;
    zint end;

    seqConvertSliceArgs(&start, &end, inclusive, info->size, startArg, endArg);

    if (start == -1) {
        return NULL;
    } else {
        return listFrom(end - start, &info->elems[start], NULL, 0, NULL);
    }
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
zvalue listFromArray(zint size, const zvalue *values) {
    for (zint i = 0; i < size; i++) {
        assertValid(values[i]);
    }

    return listFrom(size, values, NULL, 0, NULL);
}

// Documented in header.
zarray zarrayFromList(zvalue list) {
    assertHasClass(list, CLS_List);
    ListInfo *info = getInfo(list);
    return (zarray) { info->size, info->elems };
}


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_rest(List, new, values) {
    return listFromArray(valuesSize, values);
}

// Documented in spec.
METH_IMPL_rest(List, cat, args) {
    if (argsSize == 0) {
        return ths;
    }

    zint thsSize = getInfo(ths)->size;

    zint size = thsSize;
    for (zint i = 0; i < argsSize; i++) {
        zvalue one = args[i];
        assertHasClass(one, CLS_List);
        size += getInfo(one)->size;
    }

    zvalue elems[size];
    zint at = thsSize;
    arrayFromList(elems, ths);
    for (zint i = 0; i < argsSize; i++) {
        arrayFromList(&elems[at], args[i]);
        at += getInfo(args[i])->size;
    }

    return listFrom(size, elems, NULL, 0, NULL);
}

// Documented in spec.
METH_IMPL_0_1(List, collect, function) {
    if (function == NULL) {
        // Collecting a list (without filtering) results in that same list.
        return ths;
    }

    ListInfo *info = getInfo(ths);
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

// Documented in spec.
METH_IMPL_rest(List, del, ns) {
    ListInfo *info = getInfo(ths);
    zint size = info->size;
    zvalue elems[size];
    bool any = false;

    if ((nsSize == 0) || (size == 0)) {
        // Easy outs: Not actually deleting anything, and/or starting out
        // with the empty list.
        return ths;
    }

    // Make a local copy of the original elements.
    utilCpy(zvalue, elems, info->elems, size);

    // Null out the values at any valid `n` (leniently).
    for (zint i = 0; i < nsSize; i++) {
        zint index = seqNthIndexLenient(ns[i]);
        if ((index >= 0) && (index < size)) {
            any = true;
            elems[index] = NULL;
        }
    }

    if (! any) {
        // None of `ns` were in `ths`.
        return ths;
    }

    // Compact away the holes.
    zint at = 0;
    for (zint i = 0; i < size; i++) {
        if (elems[i] != NULL) {
            if (i != at) {
                elems[at] = elems[i];
            }
            at++;
        }
    }

    if (at == 0) {
        // All of the elements were removed.
        return EMPTY_LIST;
    }

    // Construct a new instance with the remaining elements.
    return listFrom(at, elems, NULL, 0, NULL);
}

// Documented in spec.
METH_IMPL_0(List, fetch) {
    ListInfo *info = getInfo(ths);

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
METH_IMPL_0(List, gcMark) {
    ListInfo *info = getInfo(ths);
    zvalue *elems = info->elems;
    zint size = info->size;

    for (zint i = 0; i < size; i++) {
        datMark(elems[i]);
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_0(List, get_size) {
    return intFromZint(getInfo(ths)->size);
}

// Documented in spec.
METH_IMPL_1(List, nextValue, box) {
    ListInfo *info = getInfo(ths);
    zint size = info->size;

    if (size == 0) {
        // `list` is empty.
        return NULL;
    }

    // Yield the first element via the box, and return a list of the
    // remainder. `listFrom` handles returning `EMPTY_LIST` when appropriate.

    cm_store(box, info->elems[0]);
    return listFrom(size - 1, &info->elems[1], NULL, 0, NULL);
}

// Documented in spec.
METH_IMPL_1(List, nth, n) {
    ListInfo *info = getInfo(ths);
    zint index = seqNthIndexStrict(info->size, n);

    return (index < 0) ? NULL : info->elems[index];
}

// Documented in spec.
METH_IMPL_2(List, put, key, value) {
    ListInfo *info = getInfo(ths);
    zvalue *elems = info->elems;
    zint size = info->size;
    zint index = seqPutIndexStrict(size, key);

    if (index == size) {
        // This is an append operation.
        return listFrom(size, elems, value, 0, NULL);
    }

    return listFrom(index, elems, value, size - index - 1, &elems[index + 1]);
}

// Documented in spec.
METH_IMPL_0(List, reverse) {
    ListInfo *info = getInfo(ths);
    zint size = info->size;
    zvalue *elems = info->elems;
    zvalue arr[size];

    for (zint i = 0, j = size - 1; i < size; i++, j--) {
        arr[i] = elems[j];
    }

    return listFrom(size, arr, NULL, 0, NULL);
}


// Documented in spec.
METH_IMPL_1_2(List, sliceExclusive, start, end) {
    return doSlice(ths, false, start, end);
}

// Documented in spec.
METH_IMPL_1_2(List, sliceInclusive, start, end) {
    return doSlice(ths, true, start, end);
}

// Documented in spec.
METH_IMPL_1(List, totalEq, other) {
    assertHasClass(other, CLS_List);  // Note: Not guaranteed to be a `List`.
    ListInfo *info1 = getInfo(ths);
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

    return ths;
}

// Documented in spec.
METH_IMPL_1(List, totalOrder, other) {
    assertHasClass(other, CLS_List);  // Note: Not guaranteed to be a `List`.
    ListInfo *info1 = getInfo(ths);
    ListInfo *info2 = getInfo(other);
    zvalue *e1 = info1->elems;
    zvalue *e2 = info2->elems;
    zint size1 = info1->size;
    zint size2 = info2->size;
    zint size = (size1 < size2) ? size1 : size2;

    for (zint i = 0; i < size; i++) {
        zorder result = cm_order(e1[i], e2[i]);
        if (result != ZSAME) {
            return symbolFromZorder(result);
        }
    }

    if (size1 == size2) {
        return SYM(same);
    }

    return (size1 < size2) ? SYM(less): SYM(more);
}

// Documented in spec.
METH_IMPL_0(List, valueList) {
    return ths;
}

/** Initializes the module. */
MOD_INIT(List) {
    MOD_USE(Sequence);

    CLS_List = makeCoreClass(SYM(List), CLS_Core,
        METH_TABLE(
            CMETH_BIND(List, new)),
        METH_TABLE(
            METH_BIND(List, cat),
            METH_BIND(List, collect),
            METH_BIND(List, del),
            METH_BIND(List, fetch),
            METH_BIND(List, gcMark),
            METH_BIND(List, get_size),
            METH_BIND(List, nextValue),
            METH_BIND(List, nth),
            METH_BIND(List, put),
            METH_BIND(List, reverse),
            METH_BIND(List, sliceExclusive),
            METH_BIND(List, sliceInclusive),
            METH_BIND(List, totalEq),
            METH_BIND(List, totalOrder),
            METH_BIND(List, valueList),
            SYM(get),        FUN_Sequence_get,
            SYM(keyList),    FUN_Sequence_keyList,
            SYM(nthMapping), FUN_Sequence_nthMapping));

    EMPTY_LIST = datImmortalize(allocList(0));
}

// Documented in header.
zvalue CLS_List = NULL;

// Documented in header.
zvalue EMPTY_LIST = NULL;
