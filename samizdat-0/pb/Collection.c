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
zchar collNthChar(zvalue coll, zint index) {
    return zcharFromString(collNth(coll, index));
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
    GFN_get = makeGeneric(2, 2, stringFromUtf8(-1, "get"));
    pbImmortalize(GFN_get);

    GFN_nth = makeGeneric(2, 2, stringFromUtf8(-1, "nth"));
    pbImmortalize(GFN_nth);

    GFN_size = makeGeneric(1, 1, stringFromUtf8(-1, "size"));
    pbImmortalize(GFN_size);
}

/* Documented in header. */
zvalue GFN_get = NULL;

/* Documented in header. */
zvalue GFN_nth = NULL;

/* Documented in header. */
zvalue GFN_size = NULL;
