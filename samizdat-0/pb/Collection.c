/*
 * Copyright 2013 the Samizdat Authors (Dan Bornstein et alia).
 * Licensed AS IS and WITHOUT WARRANTY under the Apache License,
 * Version 2.0. Details: <http://www.apache.org/licenses/LICENSE-2.0>
 */

/*
 * Collection values
 */

#include "impl.h"
#include "type/Collection.h"
#include "type/Generic.h"
#include "type/Int.h"
#include "type/String.h"
#include "type/Type.h"


/*
 * Exported Definitions
 */

/* Documented in header. */
void assertNth(zint size, zint n) {
    if (n < 0) {
        die("Invalid index (negative): %lld", n);
    }

    if (n >= size) {
        die("Invalid index: %lld; size %lld", n, size);
    }
}

/* Documented in header. */
void assertNthOrSize(zint size, zint n) {
    if (n != size) {
        assertNth(size, n);
    }
}

/* Documented in header. */
void collConvertSliceArgs(zint *startPtr, zint *endPtr, zint size,
        zint argCount, const zvalue *args) {
    if (argCount < 2) {
        die("Invalid `slice` argument count: %lld", argCount);
    }

    zvalue startVal = args[1];
    zvalue endVal = (argCount > 2) ? args[2] : NULL;
    zint start = zintFromInt(startVal);
    zint end = (endVal != NULL) ? zintFromInt(endVal) : size;

    if ((start < 0) || (end < 0) || (end < start) || (end > size)) {
        die("Invalid slice range: (%lld..!%lld) for size %lld",
            start, end, size);
    }

    *startPtr = start;
    *endPtr = end;
}

/* Documented in header. */
zvalue collGet(zvalue coll, zvalue key) {
    return GFN_CALL(get, coll, key);
}

/* Documented in header. */
bool collNthIndexLenient(zvalue key) {
    if (hasType(key, TYPE_Int)) {
        zint index = zintFromInt(key);
        return (index >= 0);
    } else {
        return false;
    }
}

/* Documented in header. */
zint collNthIndexStrict(zint size, zvalue n) {
    if (hasType(n, TYPE_Int)) {
        zint index = zintFromInt(n);
        if (index < 0) {
            die("Invalid index for nth (negative).");
        }
        return (index < size) ? index: -1;
    } else {
        die("Invalid type for nth (non-int).");
    }
}

/* Documented in header. */
zvalue collNth(zvalue coll, zint index) {
    return GFN_CALL(nth, coll, intFromZint(index));
}

/* Documented in header. */
zint collNthChar(zvalue coll, zint index) {
    zvalue result = collNth(coll, index);
    return (result == NULL) ? -1 : zcharFromString(result);
}

/* Documented in header. */
zint collSize(zvalue coll) {
    return zintFromInt(GFN_CALL(size, coll));
}


/*
 * Type Definition: `Sequence`
 *
 * **Note:** This isn't the usual form of type definition, since these
 * methods are bound on many types.
 */

/* Documented in header. */
METH_IMPL(Sequence, get) {
    zvalue seq = args[0];
    zvalue key = args[1];

    if (collNthIndexLenient(key)) {
        return GFN_CALL(nth, seq, key);
    } else {
        return NULL;
    }
}

/* Documented in header. */
void seqBind(zvalue type) {
    genericBindCore(GFN_get, type, Sequence_get);
}


/*
 * Type Definition: `Collection`
 */

/* Documented in header. */
void pbBindCollection(void) {
    GFN_cat = makeGeneric(1, -1, GFN_SAME_TYPE, stringFromUtf8(-1, "cat"));
    pbImmortalize(GFN_cat);

    GFN_del = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "del"));
    pbImmortalize(GFN_del);

    GFN_get = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "get"));
    pbImmortalize(GFN_get);

    GFN_nth = makeGeneric(2, 2, GFN_NONE, stringFromUtf8(-1, "nth"));
    pbImmortalize(GFN_nth);

    GFN_put = makeGeneric(3, 3, GFN_NONE, stringFromUtf8(-1, "put"));
    pbImmortalize(GFN_put);

    GFN_size = makeGeneric(1, 1, GFN_NONE, stringFromUtf8(-1, "size"));
    pbImmortalize(GFN_size);

    GFN_slice = makeGeneric(2, 3, GFN_NONE, stringFromUtf8(-1, "slice"));
    pbImmortalize(GFN_slice);
}

/* Documented in header. */
zvalue GFN_cat = NULL;

/* Documented in header. */
zvalue GFN_del = NULL;

/* Documented in header. */
zvalue GFN_get = NULL;

/* Documented in header. */
zvalue GFN_nth = NULL;

/* Documented in header. */
zvalue GFN_put = NULL;

/* Documented in header. */
zvalue GFN_size = NULL;

/* Documented in header. */
zvalue GFN_slice = NULL;
