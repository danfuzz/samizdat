// Copyright 2013-2015 the Samizdat Authors (Dan Bornstein et alia).
// Licensed AS IS and WITHOUT WARRANTY under the Apache License,
// Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>

#include "type/Box.h"
#include "type/Cmp.h"
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
    /** Size and pointer to elements. */
    zarray a;

    /**
     * Another list which contains the actual content, or `NULL` if the
     * content is in `content` (below). This is just used to keep the
     * elements from getting gc'ed out from under this instance.
     */
    zvalue contentList;

    /** List elements, if `contentList` is `NULL`. */
    zvalue content[/*a.size*/];
} ListInfo;

/**
 * Gets a pointer to the value's info.
 */
static ListInfo *getInfo(zvalue list) {
    return datPayload(list);
}

/**
 * Allocates an list of the given size, with built-on elements.
 */
static zvalue allocList(zint size) {
    zvalue result =
        datAllocValue(CLS_List, sizeof(ListInfo) + size * sizeof(zvalue));
    ListInfo *info = getInfo(result);

    info->a = (zarray) {size, info->content};
    info->contentList = NULL;

    return result;
}

/**
 * Makes a list that refers to a content list. Does not do any type or
 * bounds checking. It *does* shunt from an already-indirect list to the
 * ultimate bearer of content.
 */
static zvalue makeIndirectList(zvalue list, zint offset, zint size) {
    if (size == 0) {
        return EMPTY_LIST;
    }

    ListInfo *info = getInfo(list);

    if (info->contentList != NULL) {
        list = info->contentList;
    }

    zvalue result = datAllocValue(CLS_List, sizeof(ListInfo));
    ListInfo *resultInfo = getInfo(result);

    resultInfo->a = (zarray) {size, &info->a.elems[offset]};
    resultInfo->contentList = list;

    return result;
}

/**
 * Performs the main action of `listFromZarray`, except without checking
 * the validity of elements.
 */
static zvalue listFromUnchecked(zarray arr) {
    if (arr.size == 0) {
        return EMPTY_LIST;
    }

    zvalue result = allocList(arr.size);
    zvalue *content = getInfo(result)->content;

    utilCpy(zvalue, content, arr.elems, arr.size);
    return result;
}

/**
 * Helper that does most of the work of the `slice*` methods.
 */
static zvalue doSlice(zvalue ths, bool inclusive,
        zvalue startArg, zvalue endArg) {
    ListInfo *info = getInfo(ths);
    zarray arr = info->a;
    zint start;
    zint end;

    seqConvertSliceArgs(&start, &end, inclusive, arr.size, startArg, endArg);

    if (start == -1) {
        return NULL;
    }

    zint size = end - start;

    if (size > 16) {
        // Share storage for large results.
        return makeIndirectList(ths, start, size);
    } else {
        return listFromUnchecked((zarray) {end - start, &arr.elems[start]});
    }
}


//
// Exported Definitions
//

// Documented in header.
zvalue listAppend(zvalue list, zvalue elem) {
    // `EMPTY_LIST` means we know we're calling `List.cat()` and not some
    // other class's `.cat()`.
    return cm_cat(EMPTY_LIST, list, listFromValue(elem));
}

// Documented in header.
zvalue listPrepend(zvalue elem, zvalue list) {
    return cm_cat(listFromValue(elem), list);
}

// Documented in header.
zvalue listFromValue(zvalue value) {
    return listFromZarray((zarray) {1, &value});
}

// Documented in header.
zvalue listFromZarray(zarray arr) {
    if (DAT_CONSTRUCTION_PARANOIA) {
        for (zint i = 0; i < arr.size; i++) {
            assertValid(arr.elems[i]);
        }
    }

    return listFromUnchecked(arr);
}

// Documented in header.
zarray zarrayFromList(zvalue list) {
    assertHasClass(list, CLS_List);
    return getInfo(list)->a;
}


//
// Class Definition
//

// Documented in spec.
CMETH_IMPL_rest(List, new, values) {
    return listFromZarray(values);
}

// Documented in spec.
METH_IMPL_rest(List, cat, args) {
    if (args.size == 0) {
        return ths;
    }

    ListInfo *thsInfo = getInfo(ths);
    zarray thsArr = thsInfo->a;

    zint size = thsArr.size;
    for (zint i = 0; i < args.size; i++) {
        zvalue one = args.elems[i];
        assertHasClass(one, CLS_List);
        size += getInfo(one)->a.size;
    }

    zvalue elems[size];
    zint at = thsArr.size;
    utilCpy(zvalue, elems, thsArr.elems, thsArr.size);

    for (zint i = 0; i < args.size; i++) {
        zarray arr = getInfo(args.elems[i])->a;
        utilCpy(zvalue, &elems[at], arr.elems, arr.size);
        at += arr.size;
    }

    return listFromUnchecked((zarray) {size, elems});
}

// Documented in spec.
METH_IMPL_0_opt(List, collect, function) {
    if (function == NULL) {
        // Collecting a list (without filtering) results in that same list.
        return ths;
    }

    ListInfo *info = getInfo(ths);
    zarray arr = info->a;
    zvalue result[arr.size];
    zint at = 0;

    for (zint i = 0; i < arr.size; i++) {
        zvalue one = FUN_CALL(function, arr.elems[i]);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return listFromUnchecked((zarray) {at, result});
}

// Documented in spec.
METH_IMPL_1(List, crossEq, other) {
    assertHasClass(other, CLS_List);  // Note: Not guaranteed to be a `List`.
    ListInfo *info1 = getInfo(ths);
    ListInfo *info2 = getInfo(other);
    zarray arr1 = info1->a;
    zarray arr2 = info2->a;

    if (arr1.size != arr2.size) {
        return NULL;
    }

    for (zint i = 0; i < arr1.size; i++) {
        if (!cmpEq(arr1.elems[i], arr2.elems[i])) {
            return NULL;
        }
    }

    return ths;
}

// Documented in spec.
METH_IMPL_1(List, crossOrder, other) {
    assertHasClass(other, CLS_List);  // Note: Not guaranteed to be a `List`.
    ListInfo *info1 = getInfo(ths);
    ListInfo *info2 = getInfo(other);
    zarray arr1 = info1->a;
    zarray arr2 = info2->a;
    zint size = (arr1.size < arr2.size) ? arr1.size : arr2.size;

    for (zint i = 0; i < size; i++) {
        zorder result = cm_order(arr1.elems[i], arr2.elems[i]);
        if (result != ZSAME) {
            return symbolFromZorder(result);
        }
    }

    if (arr1.size == arr2.size) {
        return SYM(same);
    }

    return (arr1.size < arr2.size) ? SYM(less): SYM(more);
}

// Documented in spec.
METH_IMPL_rest(List, del, ns) {
    ListInfo *info = getInfo(ths);
    zarray arr = info->a;
    zvalue elems[arr.size];
    bool any = false;

    if ((ns.size == 0) || (arr.size == 0)) {
        // Easy outs: Not actually deleting anything, and/or starting out
        // with the empty list.
        return ths;
    }

    // Make a local copy of the original elements.
    utilCpy(zvalue, elems, info->a.elems, arr.size);

    // Null out the values at any valid `n` (leniently).
    for (zint i = 0; i < ns.size; i++) {
        zint index = seqNthIndexLenient(ns.elems[i]);
        if ((index >= 0) && (index < arr.size)) {
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
    for (zint i = 0; i < arr.size; i++) {
        if (elems[i] != NULL) {
            if (i != at) {
                elems[at] = elems[i];
            }
            at++;
        }
    }

    // Construct a new instance with the remaining elements. This call
    // handles returning `EMPTY_LIST` when appropriate.
    return listFromUnchecked((zarray) {at, elems});
}

// Documented in spec.
METH_IMPL_0(List, fetch) {
    ListInfo *info = getInfo(ths);
    zarray arr = info->a;

    switch (arr.size) {
        case 0: {
            return NULL;
        }
        case 1: {
            return arr.elems[0];
        }
        default: {
            die("Invalid to call `fetch` on list with size > 1.");
        }
    }
}

// Documented in spec.
METH_IMPL_0_opt(List, forEach, function) {
    ListInfo *info = getInfo(ths);
    zarray arr = info->a;
    zvalue result = NULL;

    if (function == NULL) {
        // Without a function, this method just returns the last element.
        return (arr.size == 0) ? NULL : arr.elems[arr.size - 1];
    }

    for (zint i = 0; i < arr.size; i++) {
        zvalue v = FUN_CALL(function, arr.elems[i]);
        if (v != NULL) {
            result = v;
        }
    }

    return result;
}

// Documented in header.
METH_IMPL_0(List, gcMark) {
    ListInfo *info = getInfo(ths);
    zarray arr = info->a;

    datMark(info->contentList);

    for (zint i = 0; i < arr.size; i++) {
        datMark(arr.elems[i]);
    }

    return NULL;
}

// Documented in spec.
METH_IMPL_0(List, get_size) {
    return intFromZint(getInfo(ths)->a.size);
}

// Documented in spec.
METH_IMPL_1(List, nextValue, box) {
    ListInfo *info = getInfo(ths);
    zarray arr = info->a;

    if (arr.size == 0) {
        // `list` is empty.
        return NULL;
    }

    // Yield the first element via the box, and return a list of the
    // remainder. `makeIndirectList` handles returning `EMPTY_LIST` when
    // appropriate.

    cm_store(box, arr.elems[0]);
    return makeIndirectList(ths, 1, arr.size - 1);
}

// Documented in spec.
METH_IMPL_1(List, nth, n) {
    ListInfo *info = getInfo(ths);
    zarray arr = info->a;
    zint index = seqNthIndexStrict(arr.size, n);

    return (index < 0) ? NULL : arr.elems[index];
}

// Documented in spec.
METH_IMPL_1(List, repeat, count) {
    ListInfo *thsInfo = getInfo(ths);
    zarray arr = thsInfo->a;
    zint n = zintFromInt(count);

    if (n < 0) {
        die("Invalid negative count for `repeat`.");
    } else if (n == 0) {
        return EMPTY_LIST;
    }

    zint size = n * arr.size;
    zvalue result = allocList(size);
    zvalue *content = getInfo(result)->content;

    for (zint i = 0; i < n; i++) {
        utilCpy(zvalue, &content[i * arr.size], arr.elems, arr.size);
    }

    return result;
}

// Documented in spec.
METH_IMPL_0(List, reverse) {
    ListInfo *info = getInfo(ths);
    zarray thsArr = info->a;
    zint size = thsArr.size;

    if (size < 2) {
        // Easy cases.
        return ths;
    }

    zvalue arr[size];

    for (zint i = 0, j = size - 1; i < size; i++, j--) {
        arr[i] = thsArr.elems[j];
    }

    return listFromUnchecked((zarray) {size, arr});
}


// Documented in spec.
METH_IMPL_1_opt(List, sliceExclusive, start, end) {
    return doSlice(ths, false, start, end);
}

// Documented in spec.
METH_IMPL_1_opt(List, sliceInclusive, start, end) {
    return doSlice(ths, true, start, end);
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
            METH_BIND(List, crossEq),
            METH_BIND(List, crossOrder),
            METH_BIND(List, del),
            METH_BIND(List, fetch),
            METH_BIND(List, forEach),
            METH_BIND(List, gcMark),
            METH_BIND(List, get_size),
            METH_BIND(List, nextValue),
            METH_BIND(List, nth),
            METH_BIND(List, repeat),
            METH_BIND(List, reverse),
            METH_BIND(List, sliceExclusive),
            METH_BIND(List, sliceInclusive),
            METH_BIND(List, valueList),
            SYM(get),          FUN_Sequence_get,
            SYM(keyList),      FUN_Sequence_keyList,
            SYM(reverseNth),   FUN_Sequence_reverseNth,
            SYM(sliceGeneral), FUN_Sequence_sliceGeneral));

    EMPTY_LIST = datImmortalize(allocList(0));
}

// Documented in header.
zvalue CLS_List = NULL;

// Documented in header.
zvalue EMPTY_LIST = NULL;
