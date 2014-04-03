/*
 * Copyright 2013-2014 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Sequence values
 */

#include "const.h"
#include "impl.h"
#include "type/Box.h"
#include "type/Builtin.h"
#include "type/Sequence.h"
#include "type/Generator.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/List.h"
#include "type/Map.h"
#include "type/String.h"
#include "type/Type.h"
#include "type/Value.h"


/*
 * Exported Definitions
 */

/* Documented in header. */
void seqConvertSliceArgs(zint *startPtr, zint *endPtr, bool inclusive,
        zint size, zint argCount, const zvalue *args) {
    if (argCount < 2) {
        die("Invalid `slice*` argument count: %lld", argCount);
    }

    zvalue startVal = args[1];
    zvalue endVal = (argCount > 2) ? args[2] : NULL;
    zint start = zintFromInt(startVal);
    zint end = (endVal != NULL) ? zintFromInt(endVal) : (size - 1);

    if (inclusive) {
        end++;
    }

    if (end < start) {
        *startPtr = -1;
        *endPtr = -1;
        return;
    }

    if (start < 0) {
        start = 0;
    }

    if (end > size) {
        end = size;
    }

    if (end > start) {
        *startPtr = start;
        *endPtr = end;
    } else {
        *startPtr = 0;
        *endPtr = 0;
    }
}

/* Documented in header. */
zint seqNthIndexLenient(zvalue key) {
    if (hasType(key, TYPE_Int)) {
        zint index = zintFromInt(key);
        return (index >= 0) ? index : -1;
    } else {
        return -1;
    }
}

/* Documented in header. */
zint seqNthIndexStrict(zint size, zvalue n) {
    if (hasType(n, TYPE_Int)) {
        zint index = zintFromInt(n);
        return ((index >= 0) && (index < size)) ? index : -1;
    } else {
        die("Invalid type for `nth` (non-int).");
    }
}

/* Documented in header. */
zint seqPutIndexStrict(zint size, zvalue n) {
    if (hasType(n, TYPE_Int)) {
        zint index = zintFromInt(n);
        if (index < 0) {
            die("Invalid index for `put` (negative).");
        } else if (index > size) {
            die("Invalid index for `put`: %lld, size %lld", index, size);
        }
        return index;
    } else {
        die("Invalid type for `put` (non-int).");
    }
}


/*
 * Type Definition: `Sequence`
 *
 * **Note:** This isn't the usual form of type definition, since these
 * methods are bound on many types.
 */

/** Builtin for `Sequence.collect`. */
static zvalue BI_Sequence_collect = NULL;

/** Builtin for `Sequence.get`. */
static zvalue BI_Sequence_get = NULL;

/** Builtin for `Sequence.keyList`. */
static zvalue BI_Sequence_keyList = NULL;

/** Builtin for `Sequence.nextValue`. */
static zvalue BI_Sequence_nextValue = NULL;

/** Builtin for `Sequence.nthMapping`. */
static zvalue BI_Sequence_nthMapping = NULL;

/* Documented in header. */
METH_IMPL(Sequence, collect) {
    zvalue coll = args[0];
    zvalue function = (argCount > 1) ? args[1] : NULL;

    if ((function == NULL) && hasType(coll, TYPE_List)) {
        // Special case: Collecting a list (without filtering) results in
        // that same list.
        return coll;
    }

    zint size = collSize(coll);
    zvalue result[size];
    zint at = 0;

    for (zint i = 0; i < size; i++) {
        zvalue elem = seqNth(coll, i);
        zvalue one = (function == NULL)
            ? elem
            : FUN_CALL(function, elem);

        if (one != NULL) {
            result[at] = one;
            at++;
        }
    }

    return listFromArray(at, result);
}

/* Documented in header. */
METH_IMPL(Sequence, get) {
    zvalue seq = args[0];
    zvalue key = args[1];

    if (seqNthIndexLenient(key) >= 0) {
        return GFN_CALL(nth, seq, key);
    } else {
        return NULL;
    }
}

/* Documented in header. */
METH_IMPL(Sequence, keyList) {
    zvalue seq = args[0];

    zint size = collSize(seq);
    zvalue elems[size];

    for (zint i = 0; i < size; i++) {
        elems[i] = intFromZint(i);
    }

    return listFromArray(size, elems);
}

/* Documented in header. */
METH_IMPL(Sequence, nextValue) {
    // This yields the first element directly (if any), and returns a
    // `SequenceGenerator` value to represent the rest.
    zvalue seq = args[0];
    zvalue box = args[1];
    zvalue first = seqNth(seq, 0);

    if (first == NULL) {
        // `seq` is empty.
        boxStore(box, NULL);
        return NULL;
    } else {
        boxStore(box, first);
        return makeValue(
            TYPE_SequenceGenerator,
            mapFromArgs(
                STR_seq,   seq,
                STR_index, intFromZint(1),
                NULL),
            NULL);
    }
}

/* Documented in header. */
METH_IMPL(Sequence, nthMapping) {
    zvalue seq = args[0];
    zvalue n = args[1];
    zvalue value = GFN_CALL(nth, seq, n);

    if (value == NULL) {
        return NULL;
    } else {
        zmapping mapping = {n, value};
        return mapFromArray(1, &mapping);
    }
}

/* Documented in header. */
void seqBind(zvalue type) {
    genericBind(GFN_collect,    type, BI_Sequence_collect);
    genericBind(GFN_get,        type, BI_Sequence_get);
    genericBind(GFN_keyList,    type, BI_Sequence_keyList);
    genericBind(GFN_nextValue,  type, BI_Sequence_nextValue);
    genericBind(GFN_nthMapping, type, BI_Sequence_nthMapping);
}

/** Initializes the module. */
MOD_INIT(Sequence) {
    MOD_USE(Collection);
    MOD_USE_NEXT(Generator);

    GFN_reverse = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "reverse"));
    datImmortalize(GFN_reverse);

    GFN_sliceExclusive = makeGeneric(2, 3, GFN_NONE,
        stringFromUtf8(-1, "sliceExclusive"));
    datImmortalize(GFN_sliceExclusive);

    GFN_sliceInclusive = makeGeneric(2, 3, GFN_NONE,
        stringFromUtf8(-1, "sliceInclusive"));
    datImmortalize(GFN_sliceInclusive);

    BI_Sequence_collect = makeBuiltin(1, 2, METH_NAME(Sequence, collect), 0,
        stringFromUtf8(-1, "Sequence.collect"));
    datImmortalize(BI_Sequence_collect);

    BI_Sequence_get = makeBuiltin(2, 2, METH_NAME(Sequence, get), 0,
        stringFromUtf8(-1, "Sequence.get"));
    datImmortalize(BI_Sequence_get);

    BI_Sequence_keyList = makeBuiltin(1, 1, METH_NAME(Sequence, keyList), 0,
        stringFromUtf8(-1, "Sequence.keyList"));
    datImmortalize(BI_Sequence_keyList);

    BI_Sequence_nextValue = makeBuiltin(2, 2, METH_NAME(Sequence, nextValue),
        0, stringFromUtf8(-1, "Sequence.nextValue"));
    datImmortalize(BI_Sequence_nextValue);

    BI_Sequence_nthMapping = makeBuiltin(1, 1, METH_NAME(Sequence, nthMapping),
        0, stringFromUtf8(-1, "Sequence.nthMapping"));
    datImmortalize(BI_Sequence_nthMapping);
}

/* Documented in header. */
zvalue GFN_reverse = NULL;

/* Documented in header. */
zvalue GFN_sliceExclusive = NULL;

/* Documented in header. */
zvalue GFN_sliceInclusive = NULL;
